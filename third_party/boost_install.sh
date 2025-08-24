#!/bin/bash

BOOST_VERSION="1.87.0"
BOOST_VERSION_UNDERSCORE=$(echo $BOOST_VERSION | tr '.' '_')
BOOST_TARBALL="boost_${BOOST_VERSION_UNDERSCORE}.tar.gz"
BOOST_URL="https://archives.boost.io/release/${BOOST_VERSION}/source/${BOOST_TARBALL}"
BOOST_SHELL_DIR=$(pwd)
BOOST_INSTALL_PREFIX="${BOOST_SHELL_DIR}/boost"

# 检查系统中是否已安装Boost
check_existing_boost() {
    echo "检查系统中是否已安装Boost..."

    # 检查头文件
    local include_paths=(
        "$BOOST_INSTALL_PREFIX/include"
    )

    # 检查头文件中的版本信息
    for path in "${include_paths[@]}"; do
        local version_file="$path/boost/version.hpp"
        if [ -f "$version_file" ]; then
            echo "发现已安装的Boost include (头文件路径: $path)"
            return 0
        fi
    done

    return 1
}

# 创建工作目录
create_boost_dir() {
    echo "在当前目录创建boost文件夹..."
    mkdir -p "$BOOST_INSTALL_PREFIX"
    if [ ! -d "$BOOST_INSTALL_PREFIX" ]; then
        echo "无法创建工作目录 $BOOST_INSTALL_PREFIX"
        exit 1
    fi
    cd "$BOOST_INSTALL_PREFIX" || exit
}

# 下载Boost源码
download_boost() {
    echo "正在下载Boost ${BOOST_VERSION}..."
    if command -v wget &> /dev/null; then
        echo "wget -c $BOOST_URL -O $BOOST_TARBALL"
        wget -c "$BOOST_URL" -O "$BOOST_TARBALL"
    else
        curl -L "$BOOST_URL" -o "$BOOST_TARBALL"
    fi

    if [ ! -f "$BOOST_TARBALL" ]; then
        echo "下载失败，请检查网络连接或URL是否正确"
        exit 1
    fi
}

extract_boost() {
    echo "正在解压Boost源码..."
    tar -xjf "$BOOST_TARBALL"
    if [ ! -d "boost_${BOOST_VERSION_UNDERSCORE}" ]; then
        echo "解压失败"
        exit 1
    fi
    cd "boost_${BOOST_VERSION_UNDERSCORE}" || exit
}

# 配置和编译Boost
build_boost() {
    echo "正在配置Boost..."
    ./bootstrap.sh --prefix="$BOOST_INSTALL_PREFIX"

    echo "正在编译Boost..."
    # 使用所有可用的CPU核心加速编译
    ./b2 -j$(nproc)

    echo "正在安装Boost到 $BOOST_INSTALL_PREFIX..."
    ./b2 install --prefix="$BOOST_INSTALL_PREFIX"
}

# 清理压缩包
cleanup_boost() {
    read -p "是否删除构建目录? (y/n) " -n 1 -r
      echo
      if [[ $REPLY =~ ^[Yy]$ ]]; then
          echo "正在清理构建文件..."
          cd "$BOOST_INSTALL_PREFIX" || exit
          echo "正在清理安装包 ($BOOST_TARBALL)..."
          rm -f "$BOOST_TARBALL"
          echo "正在清理安装包解压目录 (boost_${BOOST_VERSION_UNDERSCORE})..."
          rm -rf "boost_${BOOST_VERSION_UNDERSCORE}"
          echo "构建目录已删除"
      else
          echo "构建目录保留在: $BOOST_INSTALL_PREFIX"
      fi

}

# 验证安装
  boost_verify_installation() {
    echo "验证Boost安装..."
    if [ -f "${BOOST_INSTALL_PREFIX}/include/boost/version.hpp" ]; then
        echo "Boost ${BOOST_VERSION} 安装成功!"
        echo "安装路径: ${BOOST_INSTALL_PREFIX}"
        echo "源码路径: ${BOOST_INSTALL_PREFIX}/boost_${BOOST_VERSION_UNDERSCORE}"
    else
        echo "Boost安装可能失败，请检查错误信息"
        exit 1
    fi
}

install_boost() {
  if check_existing_boost 0; then
      echo "结束 Boost 安装"
      cd "$BOOST_SHELL_DIR" || exit
    return
  fi
  create_boost_dir
  download_boost
  extract_boost
  build_boost
  cleanup_boost
  boost_verify_installation
  # 结束后返回脚本路径
  cd "$BOOST_SHELL_DIR" || exit
}