package com.jt28.a6735.jtconfig;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.os.Message;
import android.support.v7.app.AlertDialog;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.Toast;

import com.jt28.a6735.jtconfig.Msg.Msg;
import com.jt28.a6735.jtconfig.adapter.ShopcartExpandableListViewAdapter;
import com.jt28.a6735.jtconfig.bean.GroupInfo;
import com.jt28.a6735.jtconfig.bean.ProductInfo;
import com.jt28.a6735.jtconfig.widget.SuperExpandableListView;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;
import org.json.JSONArray;
import org.json.JSONException;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.OptionalDataException;
import java.io.StreamCorruptedException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MainActivity extends BaseActivity implements ShopcartExpandableListViewAdapter.CheckInterface, View.OnClickListener, ShopcartExpandableListViewAdapter.ModifyCountInterface {

    private android.app.AlertDialog.Builder builder = null;
    private Context mContext;
    private View view_custom;
    private android.app.AlertDialog alert = null;

    private android.app.AlertDialog.Builder builder2 = null;
    private View view_custom2;
    private android.app.AlertDialog alert2 = null;

    private int i = 0;

    private Context context;
    private SuperExpandableListView exListView;
    private ShopcartExpandableListViewAdapter selva;
    private List<GroupInfo> groups = new ArrayList<>();// 组元素数据列表
    private Map<String, List<ProductInfo>> children = new HashMap<>();// 子元素数据列表

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mContext = MainActivity.this;
        //初始化Builder
        builder = new android.app.AlertDialog.Builder(mContext);
        //加载自定义的那个View,同时设置下'
        final LayoutInflater inflater = MainActivity.this.getLayoutInflater();
        view_custom = inflater.inflate(R.layout.add_tal, null, false);
        builder.setView(view_custom);
        builder.setCancelable(false);
        alert = builder.create();
        final EditText name = (EditText) view_custom.findViewById(R.id.addbar_name);
        Switch sw= (Switch) view_custom.findViewById(R.id.add_sw1);
        sw.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(b) {
                    Toast.makeText(getApplicationContext(), "开", Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(getApplicationContext(), "关", Toast.LENGTH_SHORT).show();
                }
            }
        });
        view_custom.findViewById(R.id.add_but_close).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                alert.dismiss();
            }
        });
        view_custom.findViewById(R.id.add_but_ok).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                children.get(groups.get(xiugai).getId()).add(new ProductInfo(i + "","1",name.getText().toString(),"123","继电器1",true));
                for (int i = 0; i < selva.getGroupCount(); i++) {
                    exListView.expandGroup(i);// 关键步骤3,初始化时，将ExpandableListView以展开的方式呈现
                }
                selva.notifyDataSetChanged();
                alert.dismiss();
            }
        });

        //初始化Builder
        builder2 = new android.app.AlertDialog.Builder(mContext);
        //加载自定义的那个View,同时设置下'
        final LayoutInflater inflater2 = MainActivity.this.getLayoutInflater();
        view_custom2 = inflater2.inflate(R.layout.add_bar, null, false);
        builder2.setView(view_custom2);
        builder2.setCancelable(true);
        alert2 = builder2.create();
        final EditText changj = (EditText) view_custom2.findViewById(R.id.addbar_name);
        view_custom2.findViewById(R.id.addbar_but_close).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                alert2.dismiss();
            }
        });
        view_custom2.findViewById(R.id.addbar_but_ok).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                groups.add(new GroupInfo(groups.size()+1 + "",changj.getText().toString()));
                List<ProductInfo> products = new ArrayList<>();
                children.put(groups.get(groups.size() - 1).getId(), products);// 将组元素的一个唯一值，这里取Id，作为子元素List的Key
                selva.notifyDataSetChanged();
                alert2.dismiss();
            }
        });


        Button download = (Button)findViewById(R.id.m_download);
        download.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //EventBus.getDefault().post(new Msg("1"));
            }
        });

        Button add = (Button)findViewById(R.id.m_add);
        add.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                alert2.show();
            }
        });

        Button add2 = (Button)findViewById(R.id.m_add2);
        add2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
//                boolean x = saveDataToSDcard("chatRecord.txt",groups);
//                System.out.println("lhb" + x);
//                getDataFromSDcard("chatRecord.txt");
                boolean x = writeListIntoSDcard("xyx.txt",groups);
                //System.out.println("lhb" + x);
                //groups = readListFromSdCard("xyx.txt");
            }
        });

        Button to_set = (Button)findViewById(R.id.to_set);
        to_set.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent();
                intent.setClass(MainActivity.this, ConfigActivity.class);
                MainActivity.this.startActivity(intent);
                //EventBus.getDefault().post(new Msg(groups,children));
            }
        });

        for (int i = 0; i < 2; i++) {
            groups.add(new GroupInfo(i + "","分组" + i));
            List<ProductInfo> products = new ArrayList<>();
            for (int j = 0; j <= i; j++) {
                products.add(new ProductInfo(j + "", "灯" + j,"备注" + j,String.valueOf(j),"输出"+j,false));
            }
            children.put(groups.get(i).getId(), products);// 将组元素的一个唯一值，这里取Id，作为子元素List的Key
        }

        exListView = (SuperExpandableListView) findViewById(R.id.exListView);
        selva = new ShopcartExpandableListViewAdapter(groups, children, this);
        selva.setCheckInterface(this);// 关键步骤1,设置复选框接口
        selva.setModifyCountInterface(this);// 关键步骤2,设置数量增减接口

        exListView.setAdapter(selva);

        for (int i = 0; i < selva.getGroupCount(); i++) {
            exListView.expandGroup(i);// 关键步骤3,初始化时，将ExpandableListView以展开的方式呈现
        }
    }


    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(Msg event) {
        /* Do something */
        System.out.println("lhb_a" + event.GetGroups());
//        Message message=new Message();
//        message.what=0;
//        handler.sendMessage(message);
    };

    private android.os.Handler handler = new android.os.Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case 0:
                    EventBus.getDefault().post(new Msg(groups,children));
                    break;
                case 1:
                    break;
                default:
                    break;
            }
        }
    };

    @Override
    public void checkGroup(int groupPosition, boolean isChecked) {
        GroupInfo group = groups.get(groupPosition);
        List<ProductInfo> childs = children.get(group.getId());
        for (int i = 0; i < childs.size(); i++) {
            childs.get(i).setChoosed(isChecked);
        }
//        if (isAllCheck())
//            cb_check_all.setChecked(true);
//        else
//            cb_check_all.setChecked(false);
        selva.notifyDataSetChanged();
        calculate();
    }

    @Override
    public void checkChild(int groupPosition, int childPosition, boolean isChecked) {
        boolean allChildSameState = true;// 判断改组下面的所有子元素是否是同一种状态
        GroupInfo group = groups.get(groupPosition);
        List<ProductInfo> childs = children.get(group.getId());
        for (int i = 0; i < childs.size(); i++) {
            if (childs.get(i).isChoosed() != isChecked) {
                allChildSameState = false;
                break;
            }
        }
        if (allChildSameState) {
            group.setChoosed(isChecked);// 如果所有子元素状态相同，那么对应的组元素被设为这种统一状态
        } else {
            group.setChoosed(false);// 否则，组元素一律设置为未选中状态
        }

//        if (isAllCheck())
//            cb_check_all.setChecked(true);
//        else
//            cb_check_all.setChecked(false);
        selva.notifyDataSetChanged();
        calculate();
    }

    //组添加小
    private int xiugai = 0;
    @Override
    public void checkButAdd(int groupPosition) {
        xiugai = groupPosition;
        alert.show();

    }

    //组删除
    @Override
    public void doChange(int groupPosition) {
        System.out.println("lhb删除" + groupPosition);
        List<GroupInfo> toBeDeleteGroups = new ArrayList<>();// 待删除的组元素列表
        GroupInfo group = groups.get(i);
        toBeDeleteGroups.add(groups.get(groupPosition));
        List<ProductInfo> toBeDeleteProducts = new ArrayList<>();// 待删除的子元素列表
        List<ProductInfo> childs = children.get(group.getId());
        for (int j = 0; j < childs.size(); j++) {
            if (childs.get(j).isChoosed()) {
                toBeDeleteProducts.add(childs.get(j));
            }
        }
        childs.removeAll(toBeDeleteProducts);

        groups.removeAll(toBeDeleteGroups);

        selva.notifyDataSetChanged();
        calculate();
    }

    @Override
    public void doIncrease(int groupPosition, int childPosition, View showCountView, boolean isChecked) {
//        ProductInfo product = (ProductInfo) selva.getChild(groupPosition, childPosition);
//        int currentCount = product.getCount();
//        currentCount++;
//        product.setCount(currentCount);
//        ((TextView) showCountView).setText(currentCount + "");
//
//        selva.notifyDataSetChanged();
//        calculate();
    }

    @Override
    public void doDecrease(int groupPosition, int childPosition, View showCountView, boolean isChecked) {
//        ProductInfo product = (ProductInfo) selva.getChild(groupPosition, childPosition);
//        int currentCount = product.getCount();
//        if (currentCount == 1)
//            return;
//        currentCount--;
//
//        product.setCount(currentCount);
//        ((TextView) showCountView).setText(currentCount + "");
//
//        selva.notifyDataSetChanged();
//        calculate();
    }

    @Override
    public void doChange(int groupPosition, int childPosition, View showCountView, boolean isChecked) {
        System.out.println("lhb 修改数据" + groupPosition + childPosition );
    }

    @Override
    public void onClick(View v) {
        AlertDialog alert;
        switch (v.getId()) {
//            case R.id.all_chekbox:
//                doCheckAll();
//                break;
//            case R.id.tv_go_to_pay:
//                if (totalCount == 0) {
//                    Toast.makeText(context, "请选择要支付的商品", Toast.LENGTH_LONG).show();
//                    return;
//                }
//                alert = new AlertDialog.Builder(context).create();
//                alert.setTitle("操作提示");
//                alert.setMessage("总计:\n" + totalCount + "种商品\n" + totalPrice + "元");
//                alert.setButton(DialogInterface.BUTTON_NEGATIVE, "取消", new DialogInterface.OnClickListener() {
//                    @Override
//                    public void onClick(DialogInterface dialog, int which) {
//                        return;
//                    }
//                });
//                alert.setButton(DialogInterface.BUTTON_POSITIVE, "确定", new DialogInterface.OnClickListener(){
//                    @Override
//                    public void onClick(DialogInterface dialog, int which) {
//                        return;
//                    }
//                });
//                alert.show();
//                break;
//            case R.id.tv_delete:
//                if (totalCount == 0) {
//                    Toast.makeText(context, "请选择要移除的商品", Toast.LENGTH_LONG).show();
//                    return;
//                }
//                alert = new AlertDialog.Builder(context).create();
//                alert.setTitle("操作提示");
//                alert.setMessage("您确定要将这些商品从购物车中移除吗？");
//                alert.setButton(DialogInterface.BUTTON_NEGATIVE, "取消", new DialogInterface.OnClickListener() {
//                    @Override
//                    public void onClick(DialogInterface dialog, int which) {
//                        return;
//                    }
//                });
//                alert.setButton(DialogInterface.BUTTON_POSITIVE, "确定", new DialogInterface.OnClickListener() {
//                    @Override
//                    public void onClick(DialogInterface dialog, int which)
//                    {
//                        doDelete();
//                    }
//                });
//                alert.show();
//                break;
        }
    }

    /**
     * 删除操作<br>
     * 1.不要边遍历边删除，容易出现数组越界的情况<br>
     * 2.现将要删除的对象放进相应的列表容器中，待遍历完后，以removeAll的方式进行删除
     */
    public void doDelete() {
        List<GroupInfo> toBeDeleteGroups = new ArrayList<GroupInfo>();// 待删除的组元素列表
        for (int i = 0; i < groups.size(); i++) {
            GroupInfo group = groups.get(i);
            if (group.isChoosed()) {

                toBeDeleteGroups.add(group);
            }
            List<ProductInfo> toBeDeleteProducts = new ArrayList<ProductInfo>();// 待删除的子元素列表
            List<ProductInfo> childs = children.get(group.getId());
            for (int j = 0; j < childs.size(); j++) {
                if (childs.get(j).isChoosed()) {
                    toBeDeleteProducts.add(childs.get(j));
                }
            }
            childs.removeAll(toBeDeleteProducts);
        }

        groups.removeAll(toBeDeleteGroups);

        selva.notifyDataSetChanged();
        calculate();
    }

    /** 全选与反选 */
    private void doCheckAll() {
        for (int i = 0; i < groups.size(); i++) {
//            groups.get(i).setChoosed(cb_check_all.isChecked());
//            GroupInfo group = groups.get(i);
//            List<ProductInfo> childs = children.get(group.getId());
//            for (int j = 0; j < childs.size(); j++) {
//                childs.get(j).setChoosed(cb_check_all.isChecked());
//            }
        }
        selva.notifyDataSetChanged();
        calculate();
    }

    /**
     * 统计操作<br>
     * 1.先清空全局计数器<br>
     * 2.遍历所有子元素，只要是被选中状态的，就进行相关的计算操作<br>
     * 3.给底部的textView进行数据填充
     */
    private void calculate() {
//        totalCount = 0;
//        totalPrice = 0.00;
//        for (int i = 0; i < groups.size(); i++) {
//            GroupInfo group = groups.get(i);
//            List<ProductInfo> childs = children.get(group.getId());
//            for (int j = 0; j < childs.size(); j++) {
//                ProductInfo product = childs.get(j);
//                if (product.isChoosed()) {
//                    totalCount++;
//                    totalPrice += product.getPrice() * product.getCount();
//                }
//            }
//        }
//        tv_total_price.setText("￥" + totalPrice);
//        tv_go_to_pay.setText("去支付(" + totalCount + ")");
    }

    private boolean isAllCheck() {
        for (GroupInfo group : groups) {
            if (!group.isChoosed())
                return false;
        }
        return true;
    }


    @Override
    public void onStop() {
        super.onStop();
        EventBus.getDefault().unregister(this);
    }

    @Override
    protected void onStart() {
        super.onStart();
        EventBus.getDefault().register(this);
    }

    public List<GroupInfo> readListFromSdCard(String fileName){
        if(Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)){  //检测sd卡是否存在
            List<GroupInfo> list;
            File sdCardDir = Environment.getExternalStorageDirectory();
            File sdFile = new File(sdCardDir,fileName);
            try {
                FileInputStream fis = new FileInputStream(sdFile);
                ObjectInputStream ois = new ObjectInputStream(fis);
                list = (List<GroupInfo>) ois.readObject();
                fis.close();
                ois.close();
                return list;
            } catch (StreamCorruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
                return null;
            } catch (OptionalDataException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
                return null;
            } catch (FileNotFoundException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
                return null;
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
                return null;
            } catch (ClassNotFoundException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
                return null;
            }
        }
        else {
            return null;
        }
    }
    /**
     * 将集合写入sd卡
     * @param fileName 文件名
     * @param list  集合
     * @return true 保存成功
     */
    public boolean writeListIntoSDcard(String fileName,List<GroupInfo> list){
//        if(Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)){
//            File sdCardDir = Environment.getExternalStorageDirectory();//获取sd卡目录
//            File sdFile  = new File(sdCardDir, fileName);
//            try {
//                FileOutputStream fos = new FileOutputStream(sdFile);
//                ObjectOutputStream oos = new ObjectOutputStream(fos);
//                oos.writeObject(list);//写入
//                fos.close();
//                oos.close();
//                return true;
//            } catch (FileNotFoundException e) {
//                // TODO Auto-generated catch block
//                e.printStackTrace();
//                return false;
//            } catch (IOException e) {
//                // TODO Auto-generated catch block
//                e.printStackTrace();
//                return false;
//            }
//        }
//        else {
//            return false;
//        }
        ObjectOutputStream objectOutputStream = null;
        FileOutputStream fileOutputStream = null;
        FileInputStream fileInputStream = null;
        ObjectInputStream objectInputStream = null;
        try {
            ArrayList<String> xxx = new ArrayList<>();
            for (int i = 0; i < groups.size(); i++) {
                xxx.add(groups.get(i).getId());
                xxx.add(groups.get(i).getName());
                xxx.add(String.valueOf(groups.get(i).isChoosed()));
            }
            //存入数据
            File file = new File(Environment.getExternalStorageDirectory().toString() + File.separator +"ContactBackup"+File.separator + "backup.dat");
            if (!file.getParentFile().exists()) {
                file.getParentFile().mkdirs();
            }

            if (!file.exists()) {
                file.createNewFile();
            }

            fileOutputStream= new FileOutputStream(file.toString());
            objectOutputStream= new ObjectOutputStream(fileOutputStream);
            objectOutputStream.writeObject(xxx);

            //取出数据
            fileInputStream = new FileInputStream(file.toString());
            objectInputStream = new ObjectInputStream(fileInputStream);
            ArrayList<String> savedArrayList =(ArrayList<String>) objectInputStream.readObject();
            List<GroupInfo> groups2 = new ArrayList<>();// 组元素数据列表
            for (int i = 0; i < savedArrayList.size(); i += 3) {
                System.out.println("取出的数据:" + savedArrayList.get(i).toString());
                groups2.add(new GroupInfo(savedArrayList.get(i).toString(),savedArrayList.get(i+1).toString()) );
            }
            groups = groups2;
            //initEvents();
            selva.notifyDataSetChanged();
//            List<ProductInfo> products = new ArrayList<>();
//            children.put(groups.get(groups.size() - 1).getId(), products);// 将组元素的一个唯一值，这里取Id，作为子元素List的Key
//            selva.notifyDataSetChanged();
//            alert2.dismiss();

        } catch (Exception e) {
            // TODO: handle exception
        }finally{
            if (objectOutputStream!=null) {
                try {
                    objectOutputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (fileOutputStream!=null) {
                try {
                    fileOutputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (objectInputStream!=null) {
                try {
                    objectInputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (fileInputStream!=null) {
                try {
                    fileInputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return false;
    }

    //保存数据到SD卡文件
    public boolean saveDataToSDcard(String fileName,List<GroupInfo> list){
        boolean isAvailable = false;    //SD是否可读
        FileOutputStream fileOutputStream = null;
        //创建File对象
        File file = new File(Environment.getExternalStorageDirectory(),fileName);
        //将list转成String类型
        List<String> cache = new ArrayList<String>();
        for (int i = 0; i < list.size(); i++) {
            // 取出当前的Map，转化为JSONObject
//            JSONObject obj = new JSONObject(list.get(i));
//            // 转化为字符串并添加进新的List中
//            cache.add(obj.toString());
            // 转化为字符串并添加进新的List中
            cache.add(list.get(i).getId());
            System.out.println("lhb" + list.get(i).getId());
            cache.add(list.get(i).getName());
            System.out.println("lhb" + list.get(i).getName());
            cache.add(String.valueOf(list.get(i).isChoosed()));
            System.out.println("lhb" + list.get(i).isChoosed());
        }
        // 可存储的字符串数据
        String listStr = cache.toString();

        //判断SD卡是否可读写
        if(Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState())){
            isAvailable = true;
            try {
                fileOutputStream = new FileOutputStream(file);
                fileOutputStream.write(listStr.getBytes());
                if(fileOutputStream != null){
                    fileOutputStream.close();
                }
            } catch (Exception e) {
                e.printStackTrace();
            }

        }
        return isAvailable;
    }

    //读取文件内容，并将String转成List<>
    public List<GroupInfo>  getDataFromSDcard(String fileName){
        //读取文件内容保存到resultStr
        String resultStr = null;
        File file = new File(Environment.getExternalStorageDirectory(),fileName);
        try {
            FileInputStream fileInputStream = new FileInputStream(file);
            byte[] b = new byte[fileInputStream.available()];
            fileInputStream.read(b);
            resultStr = new String(b);
            if(fileInputStream != null){
                fileInputStream.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("读文件出错");
        }
        //将读取的String结果转化成List<>
        List<GroupInfo> tempList = new ArrayList<>();
        try {
            JSONArray jsonArray = new JSONArray(resultStr);
            if (jsonArray.length()>0) {
                for (int i=0;i<jsonArray.length();i++) {
                    //JSONObject jsonObject = new JSONObject(jsonArray.get(i).toString());
                    //HashMap<String, Object> map = new HashMap<String, Object>();
                    //map.put("who", jsonObject.get("who"));
                    //map.put("chat", jsonObject.get("chat"));
                    //tempList.add(map);
                    System.out.println("lhb" + jsonArray);
                }
            }
        } catch (JSONException e){
            e.printStackTrace();
            System.out.println("转化list出错");
        }
        return tempList;
    }
}
