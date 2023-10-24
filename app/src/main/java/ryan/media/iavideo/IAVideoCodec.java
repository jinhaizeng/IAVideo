package ryan.media.iavideo;

import static ryan.utils.Constant.TAG;

import android.os.Environment;
import android.util.Log;
import android.view.Surface;

import java.io.File;

public class IAVideoCodec {
    static {
        System.loadLibrary("iavideo");
    }

    public boolean decodeToMp3() {
        String path = "/sdcard/DCIM/Camera";
        String fileName = "DCIM/Camera/share_f032ca40160c3d22598faf07728e8f51.mp4";
        String input = new File(
                Environment.getExternalStorageDirectory(),
                fileName
        ).getAbsolutePath();

        return true;
    }

    public boolean play(String input, Surface surface) {
        Log.d(TAG, "play input: " + input);
        _play(input, surface);
        return true;
    }
    private native void _init();
    private native void _decodeToMp3(String input, String output);

    private native void _play(String input, Surface surface);
}
