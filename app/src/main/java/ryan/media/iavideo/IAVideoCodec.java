package ryan.media.iavideo;

import static ryan.utils.Constant.TAG;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Environment;
import android.util.Log;
import android.view.Surface;

import java.io.File;

public class IAVideoCodec {
    private AudioTrack mAudioTrace;
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

    public boolean decodeAudio(String input) {
        Log.d(TAG, "decodeAudio input: " + input);
        File outputFile = new File("/sdcard/output_audio.aac");
        if (!outputFile.exists()) {
            try {
                outputFile.createNewFile();
            } catch (Exception e) {
                Log.d(TAG, "decodeAudio e: " + e);
            }
        }
        _playAudio(input, "/sdcard/output_audio.aac");
        return true;
    }
    public void createTrack(int sampleRateInHz,int nb_channals) {
        int channaleConfig;//通道数
        if (nb_channals == 1) {
            channaleConfig = AudioFormat.CHANNEL_OUT_MONO;
        } else if (nb_channals == 2) {
            channaleConfig = AudioFormat.CHANNEL_OUT_STEREO;
        }else {
            channaleConfig = AudioFormat.CHANNEL_OUT_MONO;
        }
        int buffersize=AudioTrack.getMinBufferSize(sampleRateInHz,
                channaleConfig, AudioFormat.ENCODING_PCM_16BIT);
        mAudioTrace = new AudioTrack(AudioManager.STREAM_MUSIC,sampleRateInHz,channaleConfig,
                AudioFormat.ENCODING_PCM_16BIT,buffersize,AudioTrack.MODE_STREAM);
        mAudioTrace.play();
    }

    //C传入音频数据
    public void playTrack(byte[] buffer, int lenth) {
        if (mAudioTrace != null && mAudioTrace.getPlayState() == AudioTrack.PLAYSTATE_PLAYING) {
            mAudioTrace.write(buffer, 0, lenth);
        }
    }

    public void getMediaInfo(String input) {
        _getMediaInfo(input);
    }

    private native void _init();
    private native void _decodeToMp3(String input, String output);

    private native void _play(String input, Surface surface);

    private native void _playAudio(String input, String output);

    private native void _getMediaInfo(String input);
}
