package ryan.utils;

import static ryan.utils.Constant.TAG;

import android.net.Uri;
import android.os.Bundle;
import android.util.Log;

import androidx.activity.result.ActivityResult;
import androidx.activity.result.ActivityResultCallback;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class OpenAlbumActivity extends AppCompatActivity {
    String filePath;
    ActivityResultLauncher launcher = registerForActivityResult(new ActivityResultContracts.GetContent(), this::handleResult);
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected void onResume() {
        super.onResume();
                launcher.launch("video/*");
    }

    private void handleResult(Uri uri) {
        filePath = FileUtils.getFilePathByUri(this, uri);
        Log.d(TAG, "图片地址：" + filePath);
    }
}
