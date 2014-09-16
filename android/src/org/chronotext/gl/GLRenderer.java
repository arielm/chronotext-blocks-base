/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE MODIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

package org.chronotext.gl;

import java.util.Vector;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import org.chronotext.gl.Touch;
import org.chronotext.Utils;

import android.opengl.GLSurfaceView;
import android.view.View;

public abstract class GLRenderer implements GLSurfaceView.Renderer
{
  public static final int REASON_RESUMED = 1;
  public static final int REASON_ATTACHED = 2;
  public static final int REASON_SHOWN = 3;
  public static final int REASON_PAUSED = 4;
  public static final int REASON_DETACHED = 5;
  public static final int REASON_HIDDEN = 6;

  protected boolean launched;
  protected boolean initialized;
  protected boolean attached;
  protected boolean paused;
  protected boolean hidden;

  protected boolean contextRenewalRequest;

  protected boolean resizeRequest;
  protected int viewportWidth;
  protected int viewportHeight;

  protected boolean startRequest;
  protected int startReason;

  protected int ticks;
  protected long t0;
  protected long now;
  protected long elapsed;

  public void onSurfaceCreated(GL10 gl, EGLConfig config)
  {
    Utils.LOGD("GLRenderer.onSurfaceCreated");
  }

  public void onSurfaceChanged(GL10 gl, int w, int h)
  {
    Utils.LOGD("GLRenderer.onSurfaceChanged: " + w + "x" + h);

    /*
     * IT IS IMPERATIVE TO CALL glViewport() UPON EACH onSurfaceChanged()
     * EVEN IF IT SEEMS THAT THE LATTER IS CALLED FAR TOO OFTEN BY THE SYSTEM
     * OTHERWISE: THE GLView WILL BE DEFORMED IN SOME SITUATIONS (E.G. RETURNING FROM SLEEP AFTER AN ORIENTATION CHANGE)
     */

    gl.glViewport(0, 0, w, h);

    resizeRequest = true;
    viewportWidth = w;
    viewportHeight = h;

    // ---

    if (launched && !initialized)
    {
      initialized = true;

      setup(gl, w, h);
      attach();
    }
  }

  public void onDrawFrame(GL10 gl)
  {
    if (contextRenewalRequest)
    {
      resizeRequest = true;
      contextRenewalRequest = false;
      contextRenewed();
    }

    if (resizeRequest)
    {
      resizeRequest = false;
      resize(gl, viewportWidth, viewportHeight);
    }

    if (startRequest)
    {
      startRequest = false;
      performStart(startReason);
    }

    // ---

    now = System.currentTimeMillis();

    if (ticks == 0)
    {
      Utils.LOGD("GLRenderer.onDrawFrame");
      t0 = now;
    }

    ticks++;
    elapsed = now - t0;

    // ---

    draw(gl);
  }

  // ---------------------------------------- LIFE-CYCLE ----------------------------------------

  protected void performLaunch()
  {
    launched = true;
    launch();
  }

  protected void performStart(int reason)
  {
    ticks = 0;
    start(reason);
  }

  protected void performStop(int reason)
  {
    Utils.LOGI("AVERAGE FRAME-RATE: " + ticks / (elapsed / 1000f) + " FRAMES PER SECOND");
    stop(reason);    
  }

  protected void requestStart(int reason)
  {
    startRequest = true;
    startReason = reason;
  }

  protected void resume()
  {
    paused = false;
    requestStart(REASON_RESUMED);
  }

  protected void pause()
  {
    paused = true;
    performStop(REASON_PAUSED);
  }

  protected void attach()
  {
    attached = true;
    requestStart(REASON_ATTACHED);
  }

  protected void detach()
  {
    attached = false;
    performStop(REASON_DETACHED);
  }

  protected void show()
  {
    hidden = false;
    requestStart(REASON_SHOWN);
  }

  protected void hide()
  {
    hidden = true;
    performStop(REASON_HIDDEN);
  }

  // ---------------------------------------- GUARANTEED TO TAKE PLACE ON THE RENDERER'S THREAD----------------------------------------

  public void contextCreated()
  {
    if (initialized)
    {
      contextRenewalRequest = true;  
    }
  }

  public void contextDestroyed()
  {
    if (initialized)
    {
      contextLost();
    }
  }

  public void onAttachedToWindow()
  {
    Utils.LOGD("GLRenderer.onAttachedToWindow");

    if (initialized && !paused && !hidden)
    {
      attach();
    }
  }

  public void onDetachedFromWindow()
  {
    Utils.LOGD("GLRenderer.onDetachedFromWindow");

    if (!paused && !hidden)
    {
      detach();
    }
  }

  public void onDestroy()
  {
    Utils.LOGD("GLRenderer.onDestroy");

    if (initialized)
    {
      shutdown();
    }
  }

  public void onVisibilityChanged(int visibility)
  {
    Utils.LOGD("GLRenderer.onVisibilityChanged: " + visibility);

    if (initialized)
    {
      switch (visibility)
      {
        case View.VISIBLE:
        {
          show();
          break;
        }

        case View.GONE:
        case View.INVISIBLE: // WARNING: THIS ONE SEEMS TO TRIGGER SOFTWARE-RENDERING ON OLDER SYSTEMS (E.G. XOOM 1 V3.1)
        {
          hide();
          break;
        }
      }
    }
  }

  public void onResume()
  {
    Utils.LOGD("GLRenderer.onResume");

    if (attached)
    {
      if (hidden)
      {
        foreground();
      }
      else
      {
         resume();
      }
    }
  }

  public void onPause()
  {
    Utils.LOGD("GLRenderer.onPause");

    if (attached)
    {
      if (hidden)
      {
        background();
      }
      else
      {
        pause();
      }
    }
  }

  // ---------------------------------------- ABSTRACT METHODS ----------------------------------------

  public abstract void launch();
  public abstract void setup(GL10 gl, int width, int height);
  public abstract void shutdown();

  public abstract void resize(GL10 gl, int width, int height);
  public abstract void draw(GL10 gl);

  public abstract void start(int reason);
  public abstract void stop(int reason);

  public abstract void foreground();
  public abstract void background();

  public abstract void contextLost();
  public abstract void contextRenewed();

  public abstract void addTouches(Vector<Touch> touches);
  public abstract void updateTouches(Vector<Touch> touches);
  public abstract void removeTouches(Vector<Touch> touches);

  public abstract void sendMessage(int what, String body);
}
