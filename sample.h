#pragma once

#include "yang.h"




// 消息id
enum MsgID
{
	
	// 请求登录
	CS_REQ_LOGIN = 1,
	
	// 返回登录
	SC_RES_LOGIN = 2,
	
};



// 常量
enum ConstDefine
{
	
	// 用户名
	ACC_LEN = 32,
	
	// 角色名
	ROLE_NAME_LEN = 32,
	
	// 密码
	PWD_LEN = 32,
	
	// 密码
	ROLE_INFO_NUM = 5,
	
	// 好友
	FRIEND_LEN = 50,
	
	// 道具
	ITEM_LEN = 100,
	
};



// 账号信息
struct AccInfo  
{
	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}

	
	
	// 1 用户名  
	char m_Acc[ACC_LEN];  
	
	// 2 钻石  
	uint32_t m_diamond;  
	  
	
	int32_t Marshal(char * buffer, int32_t size)
	{
		int ret = 0;
		
		 
		// 1 用户名	
		MARSHAL_STRING(1, m_Acc, ACC_LEN);
		 
		// 2 钻石	
		MARSHAL_NORMAL(2, m_diamond);
		
		
		return ret;
	}
	
	int32_t Unmarshal(const char * buffer, int32_t size)
	{ 
		  
		
		bool ok = false;
		
		while (size > 0)
		{
			if (!(EXPECT_TAG(1) ||
				EXPECT_TAG(2) ||
				 0))
			{
				int type = 0;
				UNMARSHAL_TAG(type);
				SKIP_DATA(type);
				continue;
			}
			
			
			// 1 用户名	
			UNMARSHAL_STRING(1, m_Acc, ACC_LEN);
			
			// 2 钻石	
			UNMARSHAL_NORMAL(2, m_diamond);
			
		}
		
		return 0;
	}
	
	
};



// 好友信息
struct FriendInfo  
{
	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}

	
	
	// 1 唯一id  
	uint64_t m_Guid;  
	
	// 2 好友的好友信息 Len
	uint16_t m_FriendNum;
	
	// 2 好友的好友信息  
	uint64_t m_Friend[FRIEND_LEN];  
	  
	
	int32_t Marshal(char * buffer, int32_t size)
	{
		int ret = 0;
		
		 
		// 1 唯一id	
		MARSHAL_NORMAL(1, m_Guid);
		 
		// 2 好友的好友信息	
		for (int32_t i = 0; i < (int32_t)m_FriendNum && i < FRIEND_LEN; i++)
		{
			MARSHAL_NORMAL(2, m_Friend[i]);
		}
		
		
		return ret;
	}
	
	int32_t Unmarshal(const char * buffer, int32_t size)
	{ 
		
		// 2 好友的好友信息 Len
		m_FriendNum = 0;
		  
		
		bool ok = false;
		
		while (size > 0)
		{
			if (!(EXPECT_TAG(1) ||
				EXPECT_TAG(2) ||
				 0))
			{
				int type = 0;
				UNMARSHAL_TAG(type);
				SKIP_DATA(type);
				continue;
			}
			
			
			// 1 唯一id	
			UNMARSHAL_NORMAL(1, m_Guid);
			
			// 2 好友的好友信息	
			while (EXPECT_TAG(2))
			{
				if ((int32_t)m_FriendNum < FRIEND_LEN)
				{
					UNMARSHAL_NORMAL(2, m_Friend[m_FriendNum]);
					if (ok)
					{
						m_FriendNum++;
					}
					else
					{
						break;
					}
				}
				else
				{
					int type = 0;
					UNMARSHAL_TAG(type);
					SKIP_DATA(type);
				}
			}
			
		}
		
		return 0;
	}
	
	
};



// 道具信息
struct ItemInfo  
{
	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}

	
	
	// 1 配置id  
	uint32_t m_Id;  
	  
	
	int32_t Marshal(char * buffer, int32_t size)
	{
		int ret = 0;
		
		 
		// 1 配置id	
		MARSHAL_NORMAL(1, m_Id);
		
		
		return ret;
	}
	
	int32_t Unmarshal(const char * buffer, int32_t size)
	{ 
		  
		
		bool ok = false;
		
		while (size > 0)
		{
			if (!(EXPECT_TAG(1) ||
				 0))
			{
				int type = 0;
				UNMARSHAL_TAG(type);
				SKIP_DATA(type);
				continue;
			}
			
			
			// 1 配置id	
			UNMARSHAL_NORMAL(1, m_Id);
			
		}
		
		return 0;
	}
	
	
};



// 玩家信息
struct RoleInfo  
{
	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}

	
	
	// 1 唯一id  
	uint64_t m_Guid;  
	
	// 2 角色名  
	char m_Name[ROLE_NAME_LEN];  
	
	// 3 血量  
	uint32_t m_Hp;  
	
	// 4 魔量  
	uint32_t m_Mp;  
	
	// 5 x  
	float m_X;  
	
	// 6 y  
	float m_Y;  
	
	// 7 好友信息 Len
	uint16_t m_FriendInfoNum;
	
	// 7 好友信息  
	FriendInfo m_FriendInfo[FRIEND_LEN];  
	
	// 8 道具信息 Len
	uint16_t m_ItemInfoNum;
	
	// 8 道具信息  
	ItemInfo m_ItemInfo[ITEM_LEN];  
	  
	
	int32_t Marshal(char * buffer, int32_t size)
	{
		int ret = 0;
		
		 
		// 1 唯一id	
		MARSHAL_NORMAL(1, m_Guid);
		 
		// 2 角色名	
		MARSHAL_STRING(2, m_Name, ROLE_NAME_LEN);
		 
		// 3 血量	
		MARSHAL_NORMAL(3, m_Hp);
		 
		// 4 魔量	
		MARSHAL_NORMAL(4, m_Mp);
		 
		// 5 x	
		MARSHAL_NORMAL(5, m_X);
		 
		// 6 y	
		MARSHAL_NORMAL(6, m_Y);
		 
		// 7 好友信息	
		for (int32_t i = 0; i < (int32_t)m_FriendInfoNum && i < FRIEND_LEN; i++)
		{
			MARSHAL_STRUCT(7, m_FriendInfo[i]);
		}
		 
		// 8 道具信息	
		for (int32_t i = 0; i < (int32_t)m_ItemInfoNum && i < ITEM_LEN; i++)
		{
			MARSHAL_STRUCT(8, m_ItemInfo[i]);
		}
		
		
		return ret;
	}
	
	int32_t Unmarshal(const char * buffer, int32_t size)
	{ 
		
		// 7 好友信息 Len
		m_FriendInfoNum = 0;
		
		// 8 道具信息 Len
		m_ItemInfoNum = 0;
		  
		
		bool ok = false;
		
		while (size > 0)
		{
			if (!(EXPECT_TAG(1) ||
				EXPECT_TAG(2) ||
				EXPECT_TAG(3) ||
				EXPECT_TAG(4) ||
				EXPECT_TAG(5) ||
				EXPECT_TAG(6) ||
				EXPECT_TAG(7) ||
				EXPECT_TAG(8) ||
				 0))
			{
				int type = 0;
				UNMARSHAL_TAG(type);
				SKIP_DATA(type);
				continue;
			}
			
			
			// 1 唯一id	
			UNMARSHAL_NORMAL(1, m_Guid);
			
			// 2 角色名	
			UNMARSHAL_STRING(2, m_Name, ROLE_NAME_LEN);
			
			// 3 血量	
			UNMARSHAL_NORMAL(3, m_Hp);
			
			// 4 魔量	
			UNMARSHAL_NORMAL(4, m_Mp);
			
			// 5 x	
			UNMARSHAL_NORMAL(5, m_X);
			
			// 6 y	
			UNMARSHAL_NORMAL(6, m_Y);
			
			// 7 好友信息	
			while (EXPECT_TAG(7))
			{
				if ((int32_t)m_FriendInfoNum < FRIEND_LEN)
				{
					UNMARSHAL_STRUCT(7, m_FriendInfo[m_FriendInfoNum]);
					if (ok)
					{
						m_FriendInfoNum++;
					}
					else
					{
						break;
					}
				}
				else
				{
					int type = 0;
					UNMARSHAL_TAG(type);
					SKIP_DATA(type);
				}
			}
			
			// 8 道具信息	
			while (EXPECT_TAG(8))
			{
				if ((int32_t)m_ItemInfoNum < ITEM_LEN)
				{
					UNMARSHAL_STRUCT(8, m_ItemInfo[m_ItemInfoNum]);
					if (ok)
					{
						m_ItemInfoNum++;
					}
					else
					{
						break;
					}
				}
				else
				{
					int type = 0;
					UNMARSHAL_TAG(type);
					SKIP_DATA(type);
				}
			}
			
		}
		
		return 0;
	}
	
	
};



// 请求登录
struct CSReqLogin  
{
	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}

	
	
	// 1 用户名  
	char m_Acc[ACC_LEN];  
	
	// 2 密码  
	char m_Pwd[PWD_LEN];  
	  
	
	int32_t Marshal(char * buffer, int32_t size)
	{
		int ret = 0;
		
		 
		// 1 用户名	
		MARSHAL_STRING(1, m_Acc, ACC_LEN);
		 
		// 2 密码	
		MARSHAL_STRING(2, m_Pwd, PWD_LEN);
		
		
		return ret;
	}
	
	int32_t Unmarshal(const char * buffer, int32_t size)
	{ 
		  
		
		bool ok = false;
		
		while (size > 0)
		{
			if (!(EXPECT_TAG(1) ||
				EXPECT_TAG(2) ||
				 0))
			{
				int type = 0;
				UNMARSHAL_TAG(type);
				SKIP_DATA(type);
				continue;
			}
			
			
			// 1 用户名	
			UNMARSHAL_STRING(1, m_Acc, ACC_LEN);
			
			// 2 密码	
			UNMARSHAL_STRING(2, m_Pwd, PWD_LEN);
			
		}
		
		return 0;
	}
	
	
};



// 返回登录
struct SCResLogin  
{
	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}

	
	
	// 1 结果  
	uint8_t m_Ret;  
	
	// 2 账号信息  
	AccInfo m_AccInfo;  
	
	// 3 玩家信息 Len
	uint16_t m_RoleInfoNum;
	
	// 3 玩家信息  
	RoleInfo m_RoleInfo[ROLE_INFO_NUM];  
	  
	
	int32_t Marshal(char * buffer, int32_t size)
	{
		int ret = 0;
		
		 
		// 1 结果	
		MARSHAL_NORMAL(1, m_Ret);
		 
		// 2 账号信息	
		
		MARSHAL_STRUCT(2, m_AccInfo);
		 
		// 3 玩家信息	
		for (int32_t i = 0; i < (int32_t)m_RoleInfoNum && i < ROLE_INFO_NUM; i++)
		{
			MARSHAL_STRUCT(3, m_RoleInfo[i]);
		}
		
		
		return ret;
	}
	
	int32_t Unmarshal(const char * buffer, int32_t size)
	{ 
		
		// 3 玩家信息 Len
		m_RoleInfoNum = 0;
		  
		
		bool ok = false;
		
		while (size > 0)
		{
			if (!(EXPECT_TAG(1) ||
				EXPECT_TAG(2) ||
				EXPECT_TAG(3) ||
				 0))
			{
				int type = 0;
				UNMARSHAL_TAG(type);
				SKIP_DATA(type);
				continue;
			}
			
			
			// 1 结果	
			UNMARSHAL_NORMAL(1, m_Ret);
			
			// 2 账号信息	
			UNMARSHAL_STRUCT(2, m_AccInfo);
			
			// 3 玩家信息	
			while (EXPECT_TAG(3))
			{
				if ((int32_t)m_RoleInfoNum < ROLE_INFO_NUM)
				{
					UNMARSHAL_STRUCT(3, m_RoleInfo[m_RoleInfoNum]);
					if (ok)
					{
						m_RoleInfoNum++;
					}
					else
					{
						break;
					}
				}
				else
				{
					int type = 0;
					UNMARSHAL_TAG(type);
					SKIP_DATA(type);
				}
			}
			
		}
		
		return 0;
	}
	
	
};



// 网络头消息
struct NetMsgHead  
{
	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}

	
	
	// 1 时间戳  
	uint32_t m_Time;  
	
	// 2 序列号  
	uint32_t m_Index;  
	  
	
	int32_t Marshal(char * buffer, int32_t size)
	{
		int ret = 0;
		
		 
		// 1 时间戳	
		MARSHAL_NORMAL(1, m_Time);
		 
		// 2 序列号	
		MARSHAL_NORMAL(2, m_Index);
		
		
		return ret;
	}
	
	int32_t Unmarshal(const char * buffer, int32_t size)
	{ 
		  
		
		bool ok = false;
		
		while (size > 0)
		{
			if (!(EXPECT_TAG(1) ||
				EXPECT_TAG(2) ||
				 0))
			{
				int type = 0;
				UNMARSHAL_TAG(type);
				SKIP_DATA(type);
				continue;
			}
			
			
			// 1 时间戳	
			UNMARSHAL_NORMAL(1, m_Time);
			
			// 2 序列号	
			UNMARSHAL_NORMAL(2, m_Index);
			
		}
		
		return 0;
	}
	
	
};



// 网络消息内容
struct NetMsgPara  
{
	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}

	
	// Type 
	int32_t m_Type;
	
	union
	{
		
		// 请求登录  
		CSReqLogin m_CSReqLogin;  
		
		// 返回登录  
		SCResLogin m_SCResLogin;  
		    
	};
	
	int32_t Marshal(char * buffer, int32_t size)
	{
		int32_t ret = 0;
		
		// type
		MARSHAL_NORMAL(1, m_Type);
		
		switch (m_Type)
		{
		  
		// 请求登录
		case CS_REQ_LOGIN:
			{
				MARSHAL_STRUCT(2, m_CSReqLogin);
			}
			break;
		  
		// 返回登录
		case SC_RES_LOGIN:
			{
				MARSHAL_STRUCT(2, m_SCResLogin);
			}
			break;
		    
		default:
			return -1;
		}
		return ret;
	}
	
	int32_t Unmarshal(const char * buffer, int32_t size)
	{
		bool ok = false;
		
		// type
		UNMARSHAL_NORMAL(1, m_Type);
		
		switch (m_Type)
		{
		    
		// 请求登录  
		case CS_REQ_LOGIN:
			{
				UNMARSHAL_STRUCT(2, m_CSReqLogin);
			}
			break;
		    
		// 返回登录  
		case SC_RES_LOGIN:
			{
				UNMARSHAL_STRUCT(2, m_SCResLogin);
			}
			break;
		    
		default:
			return -1;
		}
		return 0;
	}
	
};



// 网络消息
struct NetMsg  
{
	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}

	
	
	// 1 网络头消息  
	NetMsgHead m_NetMsgHead;  
	
	// 2 网络消息内容  
	NetMsgPara m_NetMsgPara;  
	  
	
	int32_t Marshal(char * buffer, int32_t size)
	{
		int ret = 0;
		
		 
		// 1 网络头消息	
		
		MARSHAL_STRUCT(1, m_NetMsgHead);
		 
		// 2 网络消息内容	
		
		MARSHAL_STRUCT(2, m_NetMsgPara);
		
		
		return ret;
	}
	
	int32_t Unmarshal(const char * buffer, int32_t size)
	{ 
		  
		
		bool ok = false;
		
		while (size > 0)
		{
			if (!(EXPECT_TAG(1) ||
				EXPECT_TAG(2) ||
				 0))
			{
				int type = 0;
				UNMARSHAL_TAG(type);
				SKIP_DATA(type);
				continue;
			}
			
			
			// 1 网络头消息	
			UNMARSHAL_STRUCT(1, m_NetMsgHead);
			
			// 2 网络消息内容	
			UNMARSHAL_STRUCT(2, m_NetMsgPara);
			
		}
		
		return 0;
	}
	
	
};


