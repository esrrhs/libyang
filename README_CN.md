# libyang

[<img src="https://img.shields.io/github/license/esrrhs/libyang">](https://github.com/esrrhs/libyang)
[<img src="https://img.shields.io/github/languages/top/esrrhs/libyang">](https://github.com/esrrhs/libyang)
[<img src="https://img.shields.io/github/v/release/esrrhs/libyang">](https://github.com/esrrhs/libyang/releases)
[<img src="https://img.shields.io/github/downloads/esrrhs/libyang/total">](https://github.com/esrrhs/libyang/releases)
[<img src="https://img.shields.io/github/actions/workflow/status/esrrhs/libyang/go.yml?branch=master">](https://github.com/esrrhs/libyang/actions)

> **轻量消息代码生成工具——面向 C++ 的简单粗暴版 protobuf。**

[English](README.md) | [中文说明](README_CN.md)

---

## 简介

**libyang** 读取 XML 消息定义，生成带 `Marshal` / `Unmarshal` 的 C++ 结构体。字段通过 `tag` 做版本前后兼容：解码时跳过未知 tag，缺失字段保持默认值。

生成器是纯 Go 命令行工具，不再依赖 Windows 下的 fakescript/`yang.exe`。生成代码只依赖随仓库提供的 [`yang.h`](yang.h)。

## 安装

从 **[Releases](https://github.com/esrrhs/libyang/releases)** 下载预编译二进制，或自行编译：

```bash
git clone https://github.com/esrrhs/libyang.git
cd libyang
go build -ldflags="-s -w" -o libyang .
```

Docker：

```bash
docker pull esrrhs/libyang
```

## 使用

1. 编辑 [`sample.xml`](sample.xml)（或自定义 schema）定义枚举与结构体。
2. 生成头文件：

```bash
./libyang -xml sample.xml -out sample.h
# 或
./libyang sample.xml sample.h
# 或
./run.sh
```

3. 在 C++ 工程中同时包含 `yang.h` 与生成的头文件。

查看版本：

```bash
./libyang -version
```

### Schema 示例

```xml
<result>
  <enum name="MsgID" comment="消息 id">
    <member name="CS_REQ_LOGIN" type="1" comment="请求登录"/>
  </enum>
  <struct name="CSReqLogin" comment="请求登录">
    <member name="Acc" type="char" length="ACC_LEN" comment="用户名" tag="1"/>
    <member name="Pwd" type="char" length="PWD_LEN" comment="密码" tag="2"/>
  </struct>
</result>
```

支持的成员形态：

- 数值标量（`uint32_t`、`float` 等）与嵌套结构体
- 定长 `char` 字符串（`type="char"` + `length`）
- 带运行时计数的数组（`length` + `ref`）
- 标签联合体（`struct` 的 `type="union"`，成员用 `ref` 作为 case 枚举）

## 打包发布

```bash
./pack.sh
```

跨平台压缩包输出到 `pack/`（含二进制、`yang.h`、`sample.xml` 与文档）。**Release** 流水线会监视 `version.go`：当其版本号在 `master` 上发生变更（且对应 tag 尚不存在）时，CI 会执行 `./pack.sh` 并发布 GitHub Release。

## 许可

MIT — 见 [LICENSE](LICENSE)。
