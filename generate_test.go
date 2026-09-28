package main

import (
	"os"
	"path/filepath"
	"strings"
	"testing"
)

func TestVersionNotEmpty(t *testing.T) {
	if Version == "" {
		t.Fatal("Version should not be empty")
	}
}

func TestGenerateSample(t *testing.T) {
	dir := t.TempDir()
	out := filepath.Join(dir, "sample.h")
	if err := GenerateFile("sample.xml", out); err != nil {
		t.Fatal(err)
	}
	got, err := os.ReadFile(out)
	if err != nil {
		t.Fatal(err)
	}
	text := string(got)

	mustContain := []string{
		"#include \"yang.h\"",
		"enum MsgID",
		"CS_REQ_LOGIN = 1",
		"SC_RES_LOGIN = 2",
		"enum ConstDefine",
		"ACC_LEN = 32",
		"struct AccInfo",
		"MARSHAL_STRING(1, m_Acc, ACC_LEN)",
		"MARSHAL_NORMAL(2, m_diamond)",
		"struct FriendInfo",
		"uint16_t m_FriendNum",
		"uint64_t m_Friend[FRIEND_LEN]",
		"struct RoleInfo",
		"FriendInfo m_FriendInfo[FRIEND_LEN]",
		"struct CSReqLogin",
		"struct SCResLogin",
		"struct NetMsgHead",
		"struct NetMsgPara",
		"case CS_REQ_LOGIN",
		"case SC_RES_LOGIN",
		"struct NetMsg",
		"MARSHAL_STRUCT(1, m_NetMsgHead)",
		"MARSHAL_STRUCT(2, m_NetMsgPara)",
		"UNMARSHAL_STRUCT(2, m_AccInfo)",
		"EXPECT_TAG(1)",
	}
	for _, s := range mustContain {
		if !strings.Contains(text, s) {
			t.Fatalf("generated header missing %q", s)
		}
	}

	if strings.Count(text, "struct ") < 8 {
		t.Fatalf("expected at least 8 structs, got fewer")
	}
}

func TestParsePreservesOrder(t *testing.T) {
	f, err := os.Open("sample.xml")
	if err != nil {
		t.Fatal(err)
	}
	defer f.Close()
	schema, err := ParseSchemaXML(f)
	if err != nil {
		t.Fatal(err)
	}
	if len(schema.Items) < 2 {
		t.Fatalf("expected items, got %d", len(schema.Items))
	}
	if _, ok := schema.Items[0].(Enum); !ok {
		t.Fatalf("first item should be enum, got %T", schema.Items[0])
	}
}

func TestIsNormalType(t *testing.T) {
	if !isNormalType("uint32_t") {
		t.Fatal("uint32_t should be normal")
	}
	if isNormalType("char") {
		t.Fatal("char should not be normal")
	}
	if isNormalType("AccInfo") {
		t.Fatal("struct type should not be normal")
	}
}
