package ryan.media.iavideo;

import static ryan.utils.Constant.TAG;

import android.content.Context;
import android.graphics.Color;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Gravity;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public class IAVideoView extends FrameLayout {
    private Context mContext;
    private SurfaceView mSurfaceView;

    private IAVideoPlayer iaVideoPlayer;

    public IAVideoView(@NonNull Context context) {
        super(context);
    }

    public IAVideoView(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public IAVideoView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context);
    }

    private void init(Context context) {
        mContext = context;
        setBackgroundColor(Color.BLACK);
        iaVideoPlayer = new IAVideoPlayer();
        createSurfaceView();
    }

    private void createSurfaceView() {
        mSurfaceView = new SurfaceView(mContext);
        mSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder surfaceHolder) {
//                new Thread(new Runnable() {
//                    @Override
//                    public void run() {
//                        try {
//                            Thread.currentThread().sleep(3); //快速实现功能：延迟三秒播放
//                        } catch (InterruptedException e) {
//                            throw new RuntimeException(e);
//                        }
//                        IAVideoCodec codec =new IAVideoCodec();
//                        Log.d(TAG, "surface: " + mSurfaceView.getHolder().getSurface());
//                        String path = "/sdcard/DCIM/Camera";
//                        String fileName = "DCIM/Camera/share_f032ca40160c3d22598faf07728e8f51.mp4";
//                        String input = new File(
//                                Environment.getExternalStorageDirectory(),
//                                fileName
//                        ).getAbsolutePath();
//                        codec.play("/data/user/0/ryan.media.iavideo/cache/sintel.mp4", surfaceHolder.getSurface());
////                        codec.decodeAudio(input);
//                    }
//                }).start();
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
        addView(mSurfaceView,0,layoutParams);
    }

    public void start(String path) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.currentThread().sleep(300); //快速实现功能：延迟三秒播放
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
                iaVideoPlayer.setSurface(mSurfaceView.getHolder().getSurface());
                iaVideoPlayer.setMediaPath(path);
                iaVideoPlayer.prepare();
//                IAVideoPlayer codec =new IAVideoPlayer();
//                Log.d(TAG, "surface: " + mSurfaceView.getHolder().getSurface());
//                codec.play("/data/user/0/ryan.media.iavideo/cache/sintel.mp4", mSurfaceView.getHolder().getSurface());
//                        codec.decodeAudio(input);
            }
        }).start();
    }
}
