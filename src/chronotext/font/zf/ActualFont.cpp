/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2014-2015, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

/*
 * TODO:
 *
 * 1) VERIFY THAT USING A SINGLE MEMORY-MAPPED PER FREETYPE FONT-FACE IS NOT CAUSING PROBLEMS
 *    - E.G. WHEN CRISP TEXT (I.E. NO MIPMAPPING) MUST BE RENDERED AT DIFFERENT SIZES
 *
 * 2) STUDY MEMORY-ALLOCATION IN FREETYPE
 *    - E.G. VIA THE "REPORTING" SYSTEM DESCRIBED IN mozilla-esr31/gfx/thebes/gfxAndroidPlatform.cpp
 *
 * 3) STUDY THE FOLLOWING IMPLEMENTATIONS:
 *    - mozilla-esr31/gfx/skia/trunk/src/ports/SkHarfBuzzFont.cpp
 *    - mozilla-esr31/gfx/skia/trunk/src/ports/SkFontHost_FreeType.cpp
 *    - mozilla-esr31/gfx/thebes/gfxHarfbuzzShaper.cpp
 *    - ETC.
 */

#include "chronotext/font/zf/ActualFont.h"
#include "chronotext/font/zf/FontManager.h"
#include "chronotext/Context.h"

#include "hb-ft.h"

using namespace std;
using namespace ci;

namespace chr
{
    namespace zf
    {
        /*
         See http://www.microsoft.com/typography/otspec/name.htm for a list of some
         possible platform-encoding pairs.  We're interested in 0-3 aka 3-1 - UCS-2.
         Otherwise, fail. If a font has some unicode map, but lacks UCS-2 - it is a
         broken or irrelevant font. What exactly Freetype will select on face load
         (it promises most wide unicode, and if that will be slower that UCS-2 -
         left as an excercise to check.)
         */
        static FT_Error force_ucs2_charmap(FT_Face face)
        {
            for (int i = 0; i < face->num_charmaps; i++)
            {
                auto platform_id = face->charmaps[i]->platform_id;
                auto encoding_id = face->charmaps[i]->encoding_id;
                
                if (((platform_id == 0) && (encoding_id == 3)) || ((platform_id == 3) && (encoding_id == 1)))
                {
                    return FT_Set_Charmap(face, face->charmaps[i]);
                }
            }
            
            return -1;
        }
        
        static const vector<FT_ULong> SPACE_SEPARATORS
        {
            0x0020, 0x00A0, 0x1680, 0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2007, 0x2008, 0x2009, 0x200A, 0x202F, 0x205F, 0x3000
        };
        
        // ---
        
        ActualFont::ActualFont(shared_ptr<FreetypeHelper> ftHelper, const Descriptor &descriptor, float baseSize, bool useMipmap)
        :
        ftHelper(ftHelper),
        descriptor(descriptor),
        baseSize(baseSize * descriptor.scale),
        useMipmap(useMipmap)
        {
            /*
             * PADDING IS NECESSARY IN ORDER TO AVOID BORDER ARTIFACTS
             *
             * MIPMAPPING IS EVEN ACCENTUATING THE PROBLEM
             * BECAUSE OF THE CONSECUTIVE TEXTURE SCALE-DOWNS
             */
            if (useMipmap)
            {
                padding = 2; // TODO: USE baseSize TO DEFINE AN OPTIMAL VALUE
            }
            else
            {
                padding = 1; // THE MINIMUM POSSIBLE
            }
            
            reload();
        }
        
        ActualFont::~ActualFont()
        {
            unload();
        }
        
        bool ActualFont::isSpace(hb_codepoint_t codepoint) const
        {
            return spaceSeparators.count(codepoint);
        }
        
        hb_codepoint_t ActualFont::getCodepoint(FT_ULong charCode) const
        {
            if (ftFace)
            {
                return FT_Get_Char_Index(ftFace, charCode);
            }
            else
            {
                return 0;
            }
        }
        
        const string& ActualFont::getFullName() const
        {
            return fullName;
        }
        
        bool ActualFont::reload()
        {
            if (!loaded)
            {
                MemoryInfo memoryInfo[3];

                if (FontManager::PROBE_MEMORY)
                {
                    memoryInfo[0] = getMemoryInfo();
                }
                
                // ---
                
                FT_Error error = 0;
                
                if (descriptor.forceMemoryLoad || !descriptor.inputSource->isFile())
                {
                    memoryBuffer = memoryManager().getBuffer(*descriptor.inputSource);
                    
                    if (memoryBuffer)
                    {
                        error = FT_New_Memory_Face(ftHelper->getLib(), static_cast<const FT_Byte*>(memoryBuffer->data()), memoryBuffer->size(), descriptor.faceIndex, &ftFace);
                    }
                    else
                    {
                        throw EXCEPTION(ActualFont, "UNABLE TO LOAD FONT FROM MEMORY");
                    }
                }
                else
                {
                    error = FT_New_Face(ftHelper->getLib(), descriptor.inputSource->getFilePath().c_str(), descriptor.faceIndex, &ftFace);
                }
                
                if (error)
                {
                    throw EXCEPTION(ActualFont, "FREETYPE: ERROR " + toString(error));
                }
                
                if (force_ucs2_charmap(ftFace))
                {
                    FT_Done_Face(ftFace);
                    ftFace = nullptr;
                    
                    memoryBuffer.reset();
                    
                    throw EXCEPTION(ActualFont, "HARFBUZZ: FONT IS BROKEN OR IRRELEVANT");
                }
                
                if (FontManager::PROBE_MEMORY)
                {
                    memoryInfo[1] = getMemoryInfo();
                }

                loaded = true;
                fullName = string(ftFace->family_name) + " " + ftFace->style_name;

                // ---
                
                /*
                 * USING A MATRIX WITH A MULTIPLIER ALLOWS FOR FRACTIONAL VALUES
                 * TRICK FROM http://code.google.com/p/freetype-gl/
                 *
                 * - WITHOUT A FRACTIONAL ADVANCE: CHARACTER SPACING LOOKS DUMB
                 * - WITHOUT A FRACTIONAL HEIGHT: SOME CHARACTERS WON'T BE PERFECTLY ALIGNED ON THE BASELINE
                 */
                int res = 64;
                int dpi = 72;
                
                scale = Vec2f::one() / Vec2f(res, res) / 64;
                FT_Set_Char_Size(ftFace, baseSize * 64, 0, dpi * res, dpi * res);
                
                FT_Matrix matrix =
                {
                    int((1.0 / res) * 0x10000L),
                    int((0.0) * 0x10000L),
                    int((0.0) * 0x10000L),
                    int((1.0 / res) * 0x10000L)
                };
                
                FT_Set_Transform(ftFace, &matrix, nullptr);
                
                /*
                 * THIS MUST TAKE PLACE AFTER ftFace IS PROPERLY SCALED AND TRANSFORMED
                 */
                hbFont = hb_ft_font_create(ftFace, nullptr);
                
                if (FontManager::PROBE_MEMORY)
                {
                    memoryInfo[2] = getMemoryInfo();
                }
                
                // ---
                
                metrics.height = ftFace->size->metrics.height * scale.y;
                metrics.ascent = ftFace->size->metrics.ascender * scale.y;
                metrics.descent = -ftFace->size->metrics.descender * scale.y;
                
                metrics.lineThickness = ftFace->underline_thickness / 64.0f;
                metrics.underlineOffset = -ftFace->underline_position / 64.0f;
                
                //
                
                auto os2 = static_cast<TT_OS2*>(FT_Get_Sfnt_Table(ftFace, ft_sfnt_os2));
                
                if (os2 && (os2->version != 0xFFFF) && (os2->yStrikeoutPosition != 0))
                {
                    metrics.strikethroughOffset = FT_MulFix(os2->yStrikeoutPosition, ftFace->size->metrics.y_scale) * scale.y;
                }
                else
                {
                    metrics.strikethroughOffset = 0.5f * (metrics.ascent - metrics.descent);
                }
                
                // ---
                
                /*
                 * THE FOLLOWING IS NECESSARY BECAUSE THE CODEPOINTS PROVIDED BY
                 * FREETYPE FOR SPACE-SEPARATORS ARE SOMEHOW NOT UNICODE VALUES, E.G.
                 * DEPENDING ON THE FONT, THE CODEPOINT FOR A "REGULAR SPACE" CAN BE 2 OR 3 (INSTEAD OF 32)
                 */
                if (spaceSeparators.empty())
                {
                    for (auto &separator : SPACE_SEPARATORS)
                    {
                        hb_codepoint_t codepoint(FT_Get_Char_Index(ftFace, separator));
                        
                        if (codepoint)
                        {
                            spaceSeparators.insert(codepoint);
                        }
                    }
                }
                
                // ---
                
                stringstream memoryStats;
                
                if (FontManager::PROBE_MEMORY)
                {
                    auto delta1 = memory::compare(memoryInfo[0], memoryInfo[1]);
                    auto delta2 = memory::compare(memoryInfo[1], memoryInfo[2]);
                    
                    memoryStats << " | " <<
                    utils::format::bytes(delta1) << ", " <<
                    utils::format::bytes(delta2) << " " <<
                    memoryInfo[2];
                }
                
                LOGI_IF(FontManager::LOG_VERBOSE) <<
                "LOADING ActualFont: " <<
                getFullName() << " " <<
                baseSize << (useMipmap ? " [M]" : "") <<
                " | " << (memoryBuffer ? memoryBuffer->data() : "") <<
                memoryStats.str() <<
                endl;
            }
            
            return loaded;
        }
        
        void ActualFont::unload()
        {
            if (loaded)
            {
                MemoryInfo memoryInfo[4];

                if (FontManager::PROBE_MEMORY)
                {
                    memoryInfo[0] = getMemoryInfo();
                }
                
                auto memoryUsage = getTextureMemoryUsage();
                
                // ---
                
                discardTextures();
                
                if (FontManager::PROBE_MEMORY)
                {
                    memoryInfo[1] = getMemoryInfo();
                }
                
                // ---
                
                hb_font_destroy(hbFont);
                hbFont = nullptr;

                if (FontManager::PROBE_MEMORY)
                {
                    memoryInfo[2] = getMemoryInfo();
                }

                // ---
                
                FT_Done_Face(ftFace);
                ftFace = nullptr;
                
                memoryBuffer.reset();
                
                if (FontManager::PROBE_MEMORY)
                {
                    memoryInfo[3] = getMemoryInfo();
                }

                // ---
                
                stringstream memoryStats;
                
                if (FontManager::PROBE_MEMORY)
                {
                    auto delta1 = memory::compare(memoryInfo[0], memoryInfo[1]);
                    auto delta2 = memory::compare(memoryInfo[1], memoryInfo[2]);
                    auto delta3 = memory::compare(memoryInfo[2], memoryInfo[3]);
                    
                    memoryStats << " | " <<
                    utils::format::bytes(memoryUsage) << ", " <<
                    utils::format::bytes(delta1) << ", " <<
                    utils::format::bytes(delta2) << ", " <<
                    utils::format::bytes(delta3) << " " <<
                    memoryInfo[3];
                }
                
                LOGI_IF(FontManager::LOG_VERBOSE) <<
                "UNLOADING ActualFont: " <<
                getFullName() << " " <<
                baseSize << (useMipmap ? " [M]" : "") <<
                memoryStats.str() <<
                endl;
                
                // ---
                
                loaded = false;
                fullName.clear();
            }
        }
        
        void ActualFont::discardTextures()
        {
            for (auto &texture : textures)
            {
                texture->discard();
            }
        }
        
        void ActualFont::reloadTextures()
        {
            if (reload())
            {
                for (auto &texture : textures)
                {
                    if (!texture->glId)
                    {
                        texture->upload(GlyphData(ftFace, texture->codepoint, useMipmap, padding));
                    }
                }
            }
        }
        
        int64_t ActualFont::getTextureMemoryUsage() const
        {
            int64_t total = 0;
            
            for (auto &texture : textures)
            {
                total += texture->getMemoryUsage();
            }
            
            return total;
        }
        
        ActualFont::Glyph* ActualFont::fillQuad(Quad &quad, const Shape &shape, const Vec2f &position, float sizeRatio)
        {
            auto glyph = shape.glyph; // VALID ONLY IF THE PARENT LineLayout IS LOADED
            
            if (!glyph)
            {
                glyph = getGlyph(shape.codepoint);
            }
            
            if (glyph && glyph->texture)
            {
                auto ul = position + (shape.position + glyph->offset) * sizeRatio;
                
                quad.x1 = ul.x;
                quad.y1 = ul.y,
                quad.x2 = ul.x + glyph->size.x * sizeRatio;
                quad.y2 = ul.y + glyph->size.y * sizeRatio;
                
                quad.u1 = glyph->u1;
                quad.v1 = glyph->v1;
                quad.u2 = glyph->u2;
                quad.v2 = glyph->v2;
                
                return glyph;
            }
            
            return nullptr;
        }
        
        ActualFont::Glyph* ActualFont::getGlyph(hb_codepoint_t codepoint)
        {
            Glyph *glyph = nullptr;
            
            if (reload())
            {
                auto it = glyphs.find(codepoint);
                
                if (it == glyphs.end())
                {
                    glyph = createGlyph(codepoint);
                    
                    if (glyph)
                    {
                        glyphs.emplace(codepoint, unique_ptr<Glyph>(glyph));
                    }
                    else
                    {
                        /*
                         * INSERTING A VALUE FOR TEXTURELESS GLYPHS (E.G. A "SPACE")
                         * IS NECESSARY, OTHERWISE createGlyph() WOULD BE INVOKED
                         * INDEFINITELY FOR THE SAME CODEPOINT
                         */
                        glyphs.emplace(codepoint, unique_ptr<Glyph>(new Glyph()));
                    }
                }
                else
                {
                    glyph = it->second.get();
                }
            }
            
            return glyph;
        }
        
        ActualFont::Glyph* ActualFont::createGlyph(hb_codepoint_t codepoint)
        {
            GlyphData glyphData(ftFace, codepoint, useMipmap, padding);
            
            if (glyphData.isValid())
            {
                auto texture = new FontTexture(this, codepoint, glyphData);
                textures.push_back(unique_ptr<FontTexture>(texture));
                
                return new Glyph(texture, glyphData.offset, glyphData.size);
            }
            
            return nullptr;
        }
        
        void ActualFont::reloadTexture(FontTexture *texture)
        {
            if (reload())
            {
                texture->upload(GlyphData(ftFace, texture->codepoint, useMipmap, padding));
            }
        }
    }
}
