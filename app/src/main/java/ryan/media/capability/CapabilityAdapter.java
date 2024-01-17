package ryan.media.capability;
import android.content.Intent;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;

import ryan.media.iavideo.R;

public class CapabilityAdapter extends RecyclerView.Adapter<CapabilityAdapter.CapabilityHolder> {
    private ArrayList<Capability> mDataSets;
    public CapabilityAdapter(ArrayList<Capability> dataSets) {
        mDataSets = dataSets;
    }
    @NonNull
    @Override
    public CapabilityHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.capability_item, parent, false);
        return new CapabilityHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull CapabilityHolder holder, int position) {
        Capability item = mDataSets.get(position);
        holder.mTextView.setText(item.mName);
        holder.mTextView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                Intent intent = new Intent();
                intent.setAction(item.mActivityName);
                v.getContext().startActivity(intent);
                return false;
            }
        });
    }

    @Override
    public int getItemCount() {
        return mDataSets.size();
    }

    class CapabilityHolder extends RecyclerView.ViewHolder{
        TextView mTextView;
        public CapabilityHolder(@NonNull View itemView) {
            super(itemView);
            mTextView = itemView.findViewById(R.id.capability_item_text_view);
            mTextView.setBackgroundColor(Color.rgb(0,229, 238));
        }
    }
}
