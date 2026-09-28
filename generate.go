package main

import (
	"encoding/xml"
	"fmt"
	"io"
	"os"
	"strings"
)

// SchemaItem is one top-level definition (enum or struct) in document order.
type SchemaItem interface {
	isSchemaItem()
}

// Schema is the root <result> element of a libyang message definition XML.
type Schema struct {
	Items []SchemaItem
}

type Enum struct {
	Name    string       `xml:"name,attr"`
	Comment string       `xml:"comment,attr"`
	Members []EnumMember `xml:"member"`
}

func (Enum) isSchemaItem() {}

type EnumMember struct {
	Name    string `xml:"name,attr"`
	Type    string `xml:"type,attr"`
	Comment string `xml:"comment,attr"`
}

type Struct struct {
	Name    string         `xml:"name,attr"`
	Comment string         `xml:"comment,attr"`
	Type    string         `xml:"type,attr"` // "union" or empty
	Members []StructMember `xml:"member"`
}

func (Struct) isSchemaItem() {}

type StructMember struct {
	Name    string `xml:"name,attr"`
	Type    string `xml:"type,attr"`
	Length  string `xml:"length,attr"`
	Ref     string `xml:"ref,attr"`
	Comment string `xml:"comment,attr"`
	Tag     string `xml:"tag,attr"`
}

func isNormalType(t string) bool {
	switch t {
	case "int8_t", "uint8_t", "int16_t", "uint16_t",
		"int32_t", "uint32_t", "int64_t", "uint64_t",
		"float", "double":
		return true
	default:
		return false
	}
}

// ParseSchemaXML reads a libyang XML schema from r, preserving document order.
func ParseSchemaXML(r io.Reader) (*Schema, error) {
	dec := xml.NewDecoder(r)
	var s Schema

	for {
		tok, err := dec.Token()
		if err == io.EOF {
			break
		}
		if err != nil {
			return nil, fmt.Errorf("parse xml: %w", err)
		}
		se, ok := tok.(xml.StartElement)
		if !ok {
			continue
		}
		switch se.Name.Local {
		case "result":
			// descend into children via loop
			continue
		case "enum":
			var e Enum
			if err := dec.DecodeElement(&e, &se); err != nil {
				return nil, fmt.Errorf("parse enum: %w", err)
			}
			s.Items = append(s.Items, e)
		case "struct":
			var st Struct
			if err := dec.DecodeElement(&st, &se); err != nil {
				return nil, fmt.Errorf("parse struct: %w", err)
			}
			s.Items = append(s.Items, st)
		default:
			if err := dec.Skip(); err != nil {
				return nil, fmt.Errorf("skip %s: %w", se.Name.Local, err)
			}
		}
	}
	return &s, nil
}

// GenerateHeader renders C++ message definitions from a schema.
func GenerateHeader(s *Schema) string {
	var b strings.Builder
	b.WriteString("#pragma once\n\n")
	b.WriteString("#include \"yang.h\"\n\n")

	for _, item := range s.Items {
		switch v := item.(type) {
		case Enum:
			genEnum(&b, v)
		case Struct:
			if v.Type == "union" {
				genUnionStruct(&b, v)
			} else {
				genNormalStruct(&b, v)
			}
		}
	}
	return b.String()
}

func genEnum(b *strings.Builder, e Enum) {
	fmt.Fprintf(b, "// %s\nenum %s\n{\n", e.Comment, e.Name)
	for _, m := range e.Members {
		fmt.Fprintf(b, "\t// %s\n\t%s = %s,\n", m.Comment, m.Name, m.Type)
	}
	b.WriteString("};\n\n")
}

func genNormalStruct(b *strings.Builder, st Struct) {
	fmt.Fprintf(b, "// %s\nstruct %s\n{\n", st.Comment, st.Name)
	b.WriteString("\tvoid Clear()\n\t{\n\t\tmemset(this, 0, sizeof(*this));\n\t}\n\n")

	for _, m := range st.Members {
		if m.Ref != "" {
			fmt.Fprintf(b, "\t// %s %s Len\n\tuint16_t m_%s;\n", m.Tag, m.Comment, m.Ref)
		}
		fmt.Fprintf(b, "\t// %s %s\n\t%s m_%s", m.Tag, m.Comment, m.Type, m.Name)
		if m.Length != "" {
			fmt.Fprintf(b, "[%s]", m.Length)
		}
		b.WriteString(";\n")
	}

	b.WriteString("\n\tint32_t Marshal(char * buffer, int32_t size)\n\t{\n\t\tint ret = 0;\n\n")
	for _, m := range st.Members {
		fmt.Fprintf(b, "\t\t// %s %s\n", m.Tag, m.Comment)
		switch {
		case m.Type == "char":
			fmt.Fprintf(b, "\t\tMARSHAL_STRING(%s, m_%s, %s);\n", m.Tag, m.Name, m.Length)
		case m.Length != "":
			fmt.Fprintf(b, "\t\tfor (int32_t i = 0; i < (int32_t)m_%s && i < %s; i++)\n\t\t{\n", m.Ref, m.Length)
			if isNormalType(m.Type) {
				fmt.Fprintf(b, "\t\t\tMARSHAL_NORMAL(%s, m_%s[i]);\n", m.Tag, m.Name)
			} else {
				fmt.Fprintf(b, "\t\t\tMARSHAL_STRUCT(%s, m_%s[i]);\n", m.Tag, m.Name)
			}
			b.WriteString("\t\t}\n")
		case isNormalType(m.Type):
			fmt.Fprintf(b, "\t\tMARSHAL_NORMAL(%s, m_%s);\n", m.Tag, m.Name)
		default:
			fmt.Fprintf(b, "\t\tMARSHAL_STRUCT(%s, m_%s);\n", m.Tag, m.Name)
		}
	}
	b.WriteString("\n\t\treturn ret;\n\t}\n\n")

	b.WriteString("\tint32_t Unmarshal(const char * buffer, int32_t size)\n\t{\n")
	for _, m := range st.Members {
		if m.Ref != "" {
			fmt.Fprintf(b, "\t\t// %s %s Len\n\t\tm_%s = 0;\n", m.Tag, m.Comment, m.Ref)
		}
	}
	b.WriteString("\n\t\tbool ok = false;\n\n\t\twhile (size > 0)\n\t\t{\n\t\t\tif (!(")
	for i, m := range st.Members {
		if i > 0 {
			b.WriteString(" ||\n\t\t\t\t")
		}
		fmt.Fprintf(b, "EXPECT_TAG(%s)", m.Tag)
	}
	if len(st.Members) > 0 {
		b.WriteString(" ||\n\t\t\t\t")
	}
	b.WriteString("0))\n\t\t\t{\n\t\t\t\tint type = 0;\n\t\t\t\tUNMARSHAL_TAG(type);\n\t\t\t\tSKIP_DATA(type);\n\t\t\t\tcontinue;\n\t\t\t}\n\n")

	for _, m := range st.Members {
		fmt.Fprintf(b, "\t\t\t// %s %s\n", m.Tag, m.Comment)
		switch {
		case m.Type == "char":
			fmt.Fprintf(b, "\t\t\tUNMARSHAL_STRING(%s, m_%s, %s);\n", m.Tag, m.Name, m.Length)
		case m.Length != "":
			fmt.Fprintf(b, "\t\t\twhile (EXPECT_TAG(%s))\n\t\t\t{\n", m.Tag)
			fmt.Fprintf(b, "\t\t\t\tif ((int32_t)m_%s < %s)\n\t\t\t\t{\n", m.Ref, m.Length)
			if isNormalType(m.Type) {
				fmt.Fprintf(b, "\t\t\t\t\tUNMARSHAL_NORMAL(%s, m_%s[m_%s]);\n", m.Tag, m.Name, m.Ref)
			} else {
				fmt.Fprintf(b, "\t\t\t\t\tUNMARSHAL_STRUCT(%s, m_%s[m_%s]);\n", m.Tag, m.Name, m.Ref)
			}
			b.WriteString("\t\t\t\t\tif (ok)\n\t\t\t\t\t{\n")
			fmt.Fprintf(b, "\t\t\t\t\t\tm_%s++;\n", m.Ref)
			b.WriteString("\t\t\t\t\t}\n\t\t\t\t\telse\n\t\t\t\t\t{\n\t\t\t\t\t\tbreak;\n\t\t\t\t\t}\n")
			b.WriteString("\t\t\t\t}\n\t\t\t\telse\n\t\t\t\t{\n\t\t\t\t\tint type = 0;\n\t\t\t\t\tUNMARSHAL_TAG(type);\n\t\t\t\t\tSKIP_DATA(type);\n\t\t\t\t}\n\t\t\t}\n")
		case isNormalType(m.Type):
			fmt.Fprintf(b, "\t\t\tUNMARSHAL_NORMAL(%s, m_%s);\n", m.Tag, m.Name)
		default:
			fmt.Fprintf(b, "\t\t\tUNMARSHAL_STRUCT(%s, m_%s);\n", m.Tag, m.Name)
		}
	}
	b.WriteString("\t\t}\n\n\t\treturn 0;\n\t}\n};\n\n")
}

func genUnionStruct(b *strings.Builder, st Struct) {
	fmt.Fprintf(b, "// %s\nstruct %s\n{\n", st.Comment, st.Name)
	b.WriteString("\tvoid Clear()\n\t{\n\t\tmemset(this, 0, sizeof(*this));\n\t}\n\n")
	b.WriteString("\t// Type\n\tint32_t m_Type;\n\n\tunion\n\t{\n")
	for _, m := range st.Members {
		fmt.Fprintf(b, "\t\t// %s\n\t\t%s m_%s;\n", m.Comment, m.Type, m.Name)
	}
	b.WriteString("\t};\n\n")

	b.WriteString("\tint32_t Marshal(char * buffer, int32_t size)\n\t{\n\t\tint32_t ret = 0;\n\n\t\t// type\n\t\tMARSHAL_NORMAL(1, m_Type);\n\n\t\tswitch (m_Type)\n\t\t{\n")
	for _, m := range st.Members {
		fmt.Fprintf(b, "\t\t// %s\n\t\tcase %s:\n\t\t{\n\t\t\tMARSHAL_STRUCT(2, m_%s);\n\t\t}\n\t\tbreak;\n", m.Comment, m.Ref, m.Name)
	}
	b.WriteString("\t\tdefault:\n\t\t\treturn -1;\n\t\t}\n\t\treturn ret;\n\t}\n\n")

	b.WriteString("\tint32_t Unmarshal(const char * buffer, int32_t size)\n\t{\n\t\tbool ok = false;\n\n\t\t// type\n\t\tUNMARSHAL_NORMAL(1, m_Type);\n\n\t\tswitch (m_Type)\n\t\t{\n")
	for _, m := range st.Members {
		fmt.Fprintf(b, "\t\t// %s\n\t\tcase %s:\n\t\t{\n\t\t\tUNMARSHAL_STRUCT(2, m_%s);\n\t\t}\n\t\tbreak;\n", m.Comment, m.Ref, m.Name)
	}
	b.WriteString("\t\tdefault:\n\t\t\treturn -1;\n\t\t}\n\t\treturn 0;\n\t}\n};\n\n")
}

// GenerateFile reads xmlPath and writes the generated header to outPath.
func GenerateFile(xmlPath, outPath string) error {
	f, err := os.Open(xmlPath)
	if err != nil {
		return err
	}
	defer f.Close()

	schema, err := ParseSchemaXML(f)
	if err != nil {
		return err
	}

	out := GenerateHeader(schema)
	return os.WriteFile(outPath, []byte(out), 0o644)
}
