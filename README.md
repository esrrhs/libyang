# libyang

[<img src="https://img.shields.io/github/license/esrrhs/libyang">](https://github.com/esrrhs/libyang)
[<img src="https://img.shields.io/github/languages/top/esrrhs/libyang">](https://github.com/esrrhs/libyang)
[<img src="https://img.shields.io/github/v/release/esrrhs/libyang">](https://github.com/esrrhs/libyang/releases)
[<img src="https://img.shields.io/github/downloads/esrrhs/libyang/total">](https://github.com/esrrhs/libyang/releases)
[<img src="https://img.shields.io/github/actions/workflow/status/esrrhs/libyang/go.yml?branch=master">](https://github.com/esrrhs/libyang/actions)

> **A lightweight message codegen tool — a simple alternative to Protocol Buffers for C++.**

[English](README.md) | [Chinese](README_CN.md)

---

## Overview

**libyang** reads an XML message schema and generates C++ structs with tag-based `Marshal` / `Unmarshal` helpers. Field tags enable forward/backward compatibility across versions: unknown tags are skipped on decode, and missing fields stay at defaults.

The generator is a pure Go CLI (no external scripting runtime). Generated code depends only on the bundled header [`yang.h`](yang.h).

## Install

Download a prebuilt binary from the **[Releases](https://github.com/esrrhs/libyang/releases)** page, or build from source:

```bash
git clone https://github.com/esrrhs/libyang.git
cd libyang
go build -ldflags="-s -w" -o libyang .
```

Docker:

```bash
docker pull esrrhs/libyang
```

## Usage

1. Edit [`sample.xml`](sample.xml) (or your own schema) to define enums and structs.
2. Generate the header:

```bash
./libyang -xml sample.xml -out sample.h
# or
./libyang sample.xml sample.h
# or
./run.sh
```

3. Include `yang.h` and the generated header in your C++ project.

Show version:

```bash
./libyang -version
```

### Schema sketch

```xml
<result>
  <enum name="MsgID" comment="message ids">
    <member name="CS_REQ_LOGIN" type="1" comment="login request"/>
  </enum>
  <struct name="CSReqLogin" comment="login request">
    <member name="Acc" type="char" length="ACC_LEN" comment="account" tag="1"/>
    <member name="Pwd" type="char" length="PWD_LEN" comment="password" tag="2"/>
  </struct>
</result>
```

Supported member shapes:

- Scalar numeric types (`uint32_t`, `float`, …) and nested structs
- Fixed-length `char` strings (`type="char"` + `length`)
- Arrays with a runtime count field (`length` + `ref`)
- Tagged unions (`struct` with `type="union"`, members use `ref` as the case enum)

## Build release packages

```bash
./pack.sh
```

Cross-platform archives are written to `pack/` (binary plus `yang.h`, `sample.xml`, docs). The **Release** workflow watches `version.go`: when its version string changes on `master` (and the tag does not exist yet), CI runs `./pack.sh` and publishes a GitHub Release.

## License

MIT — see [LICENSE](LICENSE).
