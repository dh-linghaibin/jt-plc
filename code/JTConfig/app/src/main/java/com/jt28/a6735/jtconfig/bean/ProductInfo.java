package com.jt28.a6735.jtconfig.bean;


public class ProductInfo extends BaseInfo
{
	private String desc;//备注
	private int position;// 绝对位置，只在ListView构造的购物车中，在删除时有效
	private String address;//地址
	private String out;//输出
	private boolean sw_bit;//状态

	public ProductInfo()
	{
		super();
	}

	public ProductInfo(String id, String name, String desc, String address,String out,boolean sw_bit)
	{

		super.Id = id;
		super.name = name;
		this.desc = desc;
		this.address = address;
		this.out = out;
		this.sw_bit = sw_bit;
	}

	public String getDesc()
	{
		return desc;
	}

	public void setDesc(String desc)
	{
		this.desc = desc;
	}

	public int getPosition()
	{
		return position;
	}

	public void setPosition(int position)
	{
		this.position = position;
	}

	public String getaddress() {
		return address;
	}
	public void setaddress(String address) {
		this.address = address;
	}

	public String getout() {
		return out;
	}
	public void setout(String out) {
		this.out = out;
	}

	public boolean getsw_bit() {
		return sw_bit;
	}
	public void setsw_bit(boolean sw_bit) {
		this.sw_bit = sw_bit;
	}
}
