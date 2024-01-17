package ryan.media.iavideo;

import static ryan.utils.Constant.TAG;

import android.content.Context;
import android.graphics.Color;
import android.os.Environment;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Gravity;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.io.File;

public class IAVideoPlayer extends FrameLayout {
    private Context mContext;
    private SurfaceView mSurfaceView;

    public IAVideoPlayer(@NonNull Context context) {
        super(context);
    }

    public IAVideoPlayer(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public IAVideoPlayer(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context);
    }

    private void init(Context context) {
        mContext = context;
        setBackgroundColor(Color.BLACK);
        createSurfaceView();
//        mAudioManager = (AudioManager)mContext.getApplicationContext().getSystemService(Context.AUDIO_SERVICE);
//        mAudioFocusHelper = new AudioFocusHelper();
    }

    private void createSurfaceView() {
        mSurfaceView = new SurfaceView(mContext);
        mSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder surfaceHolder) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            Thread.currentThread().sleep(3); //快速实现功能：延迟三秒播放
                        } catch (InterruptedException e) {
                            throw new RuntimeException(e);
                        }
                        IAVideoCodec codec =new IAVideoCodec();
                        Log.d(TAG, "surface: " + mSurfaceView.getHolder().getSurface());
                        String path = "/sdcard/DCIM/Camera";
                        String fileName = "DCIM/Camera/share_f032ca40160c3d22598faf07728e8f51.mp4";
                        String input = new File(
                                Environment.getExternalStorageDirectory(),
                                fileName
                        ).getAbsolutePath();
//                        codec.play(input, surfaceHolder.getSurface());
                        codec.decodeAudio(input);
                    }
                }).start();
            }

            @Override
            public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

            }
        });
        LayoutParams layoutParams = new LayoutParams(LayoutParams.MATCH_PARENT
                , LayoutParams.MATCH_PARENT, Gravity.CENTER);
//        mSurfaceView.setLayoutParams(layoutParams);
        addView(mSurfaceView,0,layoutParams);
    }

    public void start(String path) {

    }
}
