#! /bin/bash
set -e
NAME="libyang"

export GO111MODULE=on
export CGO_ENABLED=0

DATA_FILES=("yang.h" "sample.xml" "README.md" "LICENSE")

build_list=$(go tool dist list)

rm -rf pack
rm -f pack.zip
mkdir pack

for line in $build_list; do
  os=$(echo "$line" | awk -F"/" '{print $1}')
  arch=$(echo "$line" | awk -F"/" '{print $2}')
  echo "os=${os} arch=${arch} start build"
  if [ "$os" = "android" ] || [ "$os" = "ios" ] || [ "$arch" = "wasm" ]; then
    continue
  fi
  BIN_NAME="${NAME}"
  if [ "$os" = "windows" ]; then
    BIN_NAME="${NAME}.exe"
  fi
  GOOS=$os GOARCH=$arch go build -ldflags="-s -w" -o "${BIN_NAME}" .
  zip_args=("${BIN_NAME}")
  for f in "${DATA_FILES[@]}"; do
    if [ -f "$f" ]; then
      zip_args+=("$f")
    fi
  done
  zip -q -j "${NAME}_${os}_${arch}.zip" "${zip_args[@]}"
  mv "${NAME}_${os}_${arch}.zip" pack/
  rm -f "${BIN_NAME}"
  echo "os=${os} arch=${arch} done build"
done

zip -q -r pack.zip pack/

echo "all done"
