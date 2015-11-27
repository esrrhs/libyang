#pragma once

#include "yang.h"

{{fake func is_normal_type(str) 
	if str == "int8_t" or
		str == "uint8_t" or
		str == "int16_t" or
		str == "uint16_t" or
		str == "int32_t" or
		str == "uint32_t" or
		str == "int64_t" or
		str == "uint64_t" or
		str == "float" or
		str == "double" then
		
		return true
	
	else
		return false
	end
end
}}
{{range .memlist}}
{{if : if data["name"] == "enum" then return true else return false end}}
{{with .param}}// {{.comment}}
enum {{.name}}{{end}}
{
	{{range .memlist}}{{with .param}}
	// {{.comment}}
	{{.name}} = {{.type}}{{end}},
	{{end}}
};
{{else}}
{{with .param}}// {{.comment}}
struct {{.name}}{{end}}  
{
	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}

	{{if : p := data["param"] if p["type"] == "union" then return true else return false end}}
	// Type 
	int32_t m_Type;
	
	union
	{
		{{range .memlist}}{{with .param}}
		// {{.comment}}  
		{{.type}} m_{{.name}}{{if : if is data["length"] then return true else return false end}}[{{.length}}]{{end}};  
		{{end}}{{end}}    
	};
	
	int32_t Marshal(char * buffer, int32_t size)
	{
		int32_t ret = 0;
		
		// type
		MARSHAL_NORMAL(1, m_Type);
		
		switch (m_Type)
		{
		{{range .memlist}}{{with .param}}  
		// {{.comment}}
		case {{.ref}}:
			{
				MARSHAL_STRUCT(2, m_{{.name}});
			}
			break;
		{{end}}{{end}}    
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
		{{range .memlist}}{{with .param}}    
		// {{.comment}}  
		case {{.ref}}:
			{
				UNMARSHAL_STRUCT(2, m_{{.name}});
			}
			break;
		{{end}}{{end}}    
		default:
			return -1;
		}
		return 0;
	}
	{{else}}
	{{range .memlist}}{{with .param}}{{if : if is data["ref"] then return true else return false end}}
	// {{.tag}} {{.comment}} Len
	uint16_t m_{{.ref}};
	{{end}}
	// {{.tag}} {{.comment}}  
	{{.type}} m_{{.name}}{{if : if is data["length"] then return true else return false end}}[{{.length}}]{{end}};  
	{{end}}{{end}}  
	
	int32_t Marshal(char * buffer, int32_t size)
	{
		int ret = 0;
		
		{{range .memlist}}{{with .param}} 
		// {{.tag}} {{.comment}}	
		{{if : if data["type"] == "char" then return true else return false end}}MARSHAL_STRING({{.tag}}, m_{{.name}}, {{.length}});
		{{else}}{{if : if is data["length"] then return true else return false end}}for (int32_t i = 0; i < (int32_t)m_{{.ref}} && i < {{.length}}; i++)
		{
			{{if : if is is_normal_type(data["type"]) then return true else return false end}}MARSHAL_NORMAL({{.tag}}, m_{{.name}}[i]);{{else}}MARSHAL_STRUCT({{.tag}}, m_{{.name}}[i]);{{end}}
		}
		{{else}}{{if : if is is_normal_type(data["type"]) then return true else return false end}}MARSHAL_NORMAL({{.tag}}, m_{{.name}});{{else}}
		MARSHAL_STRUCT({{.tag}}, m_{{.name}});{{end}}
		{{end}}{{end}}{{end}}{{end}}
		
		return ret;
	}
	
	int32_t Unmarshal(const char * buffer, int32_t size)
	{ 
		{{range .memlist}}{{with .param}}{{if : if is data["ref"] then return true else return false end}}
		// {{.tag}} {{.comment}} Len
		m_{{.ref}} = 0;
		{{end}}{{end}}{{end}}  
		
		bool ok = false;
		
		while (size > 0)
		{
			if (!({{range .memlist}}{{with .param}}EXPECT_TAG({{.tag}}) ||
				{{end}}{{end}} 0))
			{
				int type = 0;
				UNMARSHAL_TAG(type);
				SKIP_DATA(type);
				continue;
			}
			
			{{range .memlist}}{{with .param}}
			// {{.tag}} {{.comment}}	
			{{if : if data["type"] == "char" then return true else return false end}}UNMARSHAL_STRING({{.tag}}, m_{{.name}}, {{.length}});
			{{else}}{{if : if is data["length"] then return true else return false end}}while (EXPECT_TAG({{.tag}}))
			{
				if ((int32_t)m_{{.ref}} < {{.length}})
				{
					{{if : if is is_normal_type(data["type"]) then return true else return false end}}UNMARSHAL_NORMAL({{.tag}}, m_{{.name}}[m_{{.ref}}]);{{else}}UNMARSHAL_STRUCT({{.tag}}, m_{{.name}}[m_{{.ref}}]);{{end}}
					if (ok)
					{
						m_{{.ref}}++;
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
			{{else}}{{if : if is is_normal_type(data["type"]) then return true else return false end}}UNMARSHAL_NORMAL({{.tag}}, m_{{.name}});{{else}}UNMARSHAL_STRUCT({{.tag}}, m_{{.name}});{{end}}
			{{end}}{{end}}{{end}}{{end}}
		}
		
		return 0;
	}
	
	{{end}}
};
{{end}}
{{end}}
