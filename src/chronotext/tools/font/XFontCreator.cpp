/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2015, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#include "XFontCreator.h"
#include "BinPack.h"

#include "chronotext/utils/Utils.h"

using namespace std;
using namespace ci;
using namespace chr;

XFontCreator::XFontCreator(shared_ptr<FreetypeHelper> ftHelper, const FontDescriptor &descriptor, float baseSize, const u16string &characters, const XParams &params)
:
baseSize(baseSize),
params(params)
{
    FT_Error error = FT_New_Face(ftHelper->getLib(), descriptor.filePath.string().c_str(), descriptor.faceIndex, &ftFace);
    
    if (error)
    {
        throw runtime_error("FREETYPE: ERROR " + toString(error));
    }
    
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
    
    FT_Select_Charmap(ftFace, FT_ENCODING_UNICODE);
    FT_Set_Char_Size(ftFace, baseSize * 64, 0, dpi * res, dpi * res);
    
    FT_Matrix matrix =
    {
        int((1.0 / res) * 0x10000L),
        int((0.0) * 0x10000L),
        int((0.0) * 0x10000L),
        int((1.0 / res) * 0x10000L)
    };
    
    FT_Set_Transform(ftFace, &matrix, NULL);
    
    // ---
    
    height = ftFace->size->metrics.height / 64.0f / res;
    ascent = ftFace->size->metrics.ascender / 64.0f / res;
    descent = -ftFace->size->metrics.descender / 64.0f / res;

    lineThickness = ftFace->underline_thickness / 64.0f;
    underlineOffset = -ftFace->underline_position / 64.0f;
    
    // ----

    TT_OS2 *os2 = (TT_OS2*)FT_Get_Sfnt_Table(ftFace, ft_sfnt_os2);
    
    if (os2 && (os2->version != 0xFFFF) && (os2->yStrikeoutPosition != 0))
    {
        strikethroughOffset = FT_MulFix(os2->yStrikeoutPosition, ftFace->size->metrics.y_scale) / 64.0f / res;
    }
    else
    {
        strikethroughOffset = 0.5f * (ascent - descent);
    }
    
    // ---
    
    FT_UInt spaceGlyphIndex = FT_Get_Char_Index(ftFace, u' ');
    FT_Load_Glyph(ftFace, spaceGlyphIndex, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT);
    spaceWidth = ftFace->glyph->advance.x / 64.0f;
    
    // ---
    
    for (auto c : characters)
    {
        if (!isSpace(c) && canDisplay(c))
        {
            if (glyphs.find(c) == glyphs.end())
            {
                XGlyph *glyph = createGlyph(c);
                
                if (glyph)
                {
                    glyphs[c] = glyph;
                    ordered.push_back(glyph);
                }
            }
        }
    }
    
    ordered.sort(XGlyph::compareSize);
    
    if (!bestFit())
    {
        throw runtime_error("FONT CREATION: ATLAS OVERFLOW");
    }
}

XFontCreator::~XFontCreator()
{
    for (auto it : glyphs)
    {
        delete it.second;
    }
    
    FT_Done_Face(ftFace);
}

void XFontCreator::writeToFolder(const fs::path &folderPath)
{
    string fileName = string(ftFace->family_name) + "_" + string(ftFace->style_name) + "_" + boost::lexical_cast<string>(baseSize) + ".fnt";
    write(writeFile(folderPath / fileName));
    
    LOGI << "GENERATED: " << fileName << " - " << atlasWidth << "x" << atlasHeight << endl;
}

void XFontCreator::write(DataTargetRef target)
{
    OStreamRef out = target->getStream();
    
    string version = "XFONT.004";
    out->write(version);
    
    int glyphCount = glyphs.size();
    out->writeLittle(glyphCount);
    
    out->writeLittle(baseSize);
    out->writeLittle(height);
    out->writeLittle(ascent);
    out->writeLittle(descent);
    out->writeLittle(lineThickness);
    out->writeLittle(underlineOffset);
    out->writeLittle(strikethroughOffset);
    out->writeLittle(spaceWidth);
    
    out->writeLittle(atlasWidth);
    out->writeLittle(atlasHeight);
    out->writeLittle(params.atlasPadding);
    out->writeLittle(params.unitMargin);
    out->writeLittle(params.unitPadding);
    
    for (auto it : glyphs)
    {
        out->writeLittle((int)it.first);
        
        XGlyph *glyph = it.second;
        out->writeLittle(glyph->advance);
        out->writeLittle(glyph->width);
        out->writeLittle(glyph->height);
        out->writeLittle(glyph->leftExtent);
        out->writeLittle(glyph->topExtent);
        out->writeLittle(glyph->atlasX);
        out->writeLittle(glyph->atlasY);
        out->writeData(glyph->data, glyph->dataSize);
    }
}

XGlyph* XFontCreator::createGlyph(char16_t c)
{
    auto glyphIndex = FT_Get_Char_Index(ftFace, c);
    
    if (glyphIndex)
    {
        auto error =  FT_Load_Glyph(ftFace, glyphIndex, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT);
        
        if (!error)
        {
            auto slot = ftFace->glyph;
            
            FT_Glyph glyph;
            error = FT_Get_Glyph(slot, &glyph);
            
            if (!error)
            {
                XGlyph *g = NULL;
                FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
                
                auto width = slot->bitmap.width;
                auto height = slot->bitmap.rows;

                if (width * height > 0)
                {
                    g = new XGlyph(slot->bitmap.buffer, width, height);
                    g->leftExtent = slot->bitmap_left;
                    g->topExtent = slot->bitmap_top;
                    g->advance = slot->advance.x / 64.0f;
                }
                
                FT_Done_Glyph(glyph);
                return g;
            }
        }
    }
    
    return NULL;
}

bool XFontCreator::canDisplay(char16_t c)
{
    return (c > 0) && FT_Get_Char_Index(ftFace, c);
}

bool XFontCreator::isSpace(char16_t c)
{
    return (c == 0x20 || c == 0xa0);
}

/*
 * VERY BASIC SYSTEM FOR AUTOMATICALLY DEFINING THE
 * DIMENSION OF THE ATLAS BASED ON CONTENT
 *
 * THE CURRENT SYSTEM GIVES "GROW-PRIORITY"
 * TO THE VERTICAL AXIS
 */
bool XFontCreator::bestFit()
{
    int minW = 16;
    int minH = 16;
    
    int maxW = params.maxAtlasWidth;
    int maxH = params.maxAtlasHeight;
    
    int bestW = minW;
    int bestH = minH;
    while (bestW <= maxW)
    {
        bestH = minH;
        while (bestH <= maxH)
        {
            if (pack(bestW, bestH))
            {
                atlasWidth = bestW;
                atlasHeight = bestH;
                return true;
            }
            
            if (bestH == maxH) break;
            bestH <<= 1;
        }
        if (bestW == maxW) break;
        bestW <<= 1;
    }
    
    atlasWidth = bestW;
    atlasHeight = bestH;
    return pack(bestW, bestH);
}

bool XFontCreator::pack(int targetWidth, int targetHeight)
{
    int atlasPaddingExtra = 2 * params.atlasPadding;
    int unitMarginExtra = 2 * params.unitMargin;
    
    BinPackRef pack = BinPack::create(targetWidth - atlasPaddingExtra, targetHeight - atlasPaddingExtra, BinPack::SKYLINE);
    
    for (auto glyph : ordered)
    {
        Area area = pack->allocateArea(glyph->width + unitMarginExtra, glyph->height + unitMarginExtra);
        
        if (area.x1 < 0)
        {
            return false;
        }
        else
        {
            glyph->atlasX = area.x1;
            glyph->atlasY = area.y1;
        }
    }
    
    return true;
}
