
package org.chronotext.Scalability;

import org.chronotext.cinder.CinderDelegate;
import org.chronotext.Utils;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.Bundle;

public class MainActivity extends Activity
{
  static
  {
    System.loadLibrary("Scalability");
  }

  CinderDelegate delegate;

  @Override
  public void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);

    delegate = new CinderDelegate(this, null);
    setContentView(delegate.getView());
  }

  @Override
  protected void onPause()
  {
    super.onPause();
    delegate.onPause();
  }

  @Override
  protected void onResume()
  {
    super.onResume();
    delegate.onResume();
  }
  
  @Override
  public void onBackPressed()
  {
    if (!delegate.onBackPressed())
    {
      super.onBackPressed();
    }
  }
}
