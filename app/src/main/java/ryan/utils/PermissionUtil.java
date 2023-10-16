package ryan.utils;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Environment;
import android.provider.Settings;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

public class PermissionUtil {
    public static void requestMyPermissions(Activity activity) {
        if (ContextCompat.checkSelfPermission(activity,
                Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            //没有授权，编写申请权限代码
            ActivityCompat.requestPermissions(activity, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 100);
        } else {
//            Log.d(Constant.TAG, "requestMyPermissions: 有写SD权限");
        }
        if (ContextCompat.checkSelfPermission(activity,
                Manifest.permission.READ_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            //没有授权，编写申请权限代码
            ActivityCompat.requestPermissions(activity, new String[] { Manifest.permission.READ_EXTERNAL_STORAGE}, 100);
        } else {
//            Log.d(Constant.TAG, "requestMyPermissions: 有读SD权限");
        }

        // android 11的限制，要打开Sdcard的公共路径需要申请权限https://blog.csdn.net/rzleilei/article/details/122700219
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!Environment.isExternalStorageManager()) {
                Intent intent = new Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION);
                activity.startActivity(intent);
            }
        }

    }
}
