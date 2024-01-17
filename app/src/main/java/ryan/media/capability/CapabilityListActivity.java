package ryan.media.capability;

import android.app.Activity;
import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;

import ryan.media.iavideo.databinding.CapabilityListBinding;

public class CapabilityListActivity extends Activity {
    private CapabilityListBinding mBinding;
    private RecyclerView mCapabilityRecycleView;

    private void initView() {
        mBinding = CapabilityListBinding.inflate(getLayoutInflater());
        setContentView(mBinding.getRoot());
        mCapabilityRecycleView = mBinding.capabilityRecycleView;
        mCapabilityRecycleView.setLayoutManager(new LinearLayoutManager(this));
        ArrayList<Capability> dataList = new ArrayList<>();
        dataList.add(new Capability("选择视频源", "open_album"));
        dataList.add(new Capability("提取音频", "ijk_video_player"));
        dataList.add(new Capability("获取文件信息", "get_media_info"));
        dataList.add(new Capability("敬请期待", ""));
        mCapabilityRecycleView.setAdapter(new CapabilityAdapter(dataList));
        mCapabilityRecycleView.addItemDecoration(new SpacesItemDecoration(20));
    }
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        initView();
    }
}
