#!/bin/bash

# gRPC 安装脚本
# 支持 Ubuntu, CentOS 和 macOS 系统

# 配置参数
GRPC_VERSION="v1.75.x"  # 可修改为需要的版本
GRPC_BUILD_TYPE="Release"
GRPC_SHELL_DIR=$(pwd)
GRPC_INSTALL_PREFIX="${GRPC_SHELL_DIR}/grpc-cpp"
PROTOBUF_INSTALL_PREFIX="${GRPC_SHELL_DIR}/protobuf"

check_existing_grpc() {
    echo "检查系统中是否已安装gRPC..."

    if command -v grpc_cpp_plugin &> /dev/null && command -v protoc &> /dev/null; then
            local grpc_version=$(grpc_cpp_plugin --version | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -n1)
            local protobuf_version=$(protoc --version | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -n1)

            if [ -n "$grpc_version" ]; then
                echo "发现已安装的gRPC版本: $grpc_version"
                echo "发现已安装的Protocol Buffers版本: $protobuf_version"
                read -p "是否继续安装? (y/n) " -n 1 -r
                echo
                if [[ ! $REPLY =~ ^[Yy]$ ]]; then
                    return 0
                fi
            fi
        else
            echo "未发现已安装的gRPC，将进行安装。"
        fi
    return 1
}

# 检查并安装依赖
install_grpc_dependencies() {
    echo "正在安装必要的依赖..."

    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # Ubuntu/Debian
        if command -v apt &> /dev/null; then
            sudo apt update
            sudo apt install -y build-essential autoconf libtool pkg-config \
                cmake git gcc g++ clang libc++-dev
        # CentOS/RHEL
        elif command -v yum &> /dev/null; then
            sudo yum install -y gcc-c++ make autoconf libtool pkgconfig \
                cmake git clang libcxx-devel
        else
            echo "不支持的Linux发行版，请手动安装依赖"
            exit 1
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS
        if ! command -v brew &> /dev/null; then
            echo "未安装Homebrew，正在安装..."
            /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        fi
        brew install autoconf automake libtool pkg-config cmake git c-ares
    else
        echo "不支持的操作系统"
        exit 1
    fi
}

# 创建工作目录
create_grpc_dir() {
    echo "创建grpc-cpp目录..."
    mkdir -p "$GRPC_INSTALL_PREFIX"
    if [ ! -d "$GRPC_INSTALL_PREFIX" ]; then
        echo "无法创建工作目录 $GRPC_INSTALL_PREFIX"
        exit 1
    fi
    cd "$GRPC_INSTALL_PREFIX" || exit
}

# 克隆gRPC仓库和子模块
  clone_grpc_repositories() {
    echo "正在克隆gRPC仓库 (版本: $GRPC_VERSION)..."

    if [ -d "grpc" ]; then
        echo "gRPC仓库已存在，更新代码..."
#        cd grpc || exit
#        git checkout $GRPC_VERSION
#        git submodule update --init --recursive
#        cd ..
    else
        git clone --recurse-submodules -b $GRPC_VERSION https://github.com/grpc/grpc.git
        if [ ! -d "grpc" ]; then
            echo "克隆gRPC仓库失败"
            exit 1
        fi
    fi
}

# 检查并安装protobuf
install_protobuf() {
    echo "=== 开始安装 Protocol Buffers ==="

    # 检查是否已安装正确版本的protobuf
    # 检查头文件
    local bin_paths=(
        "$PROTOBUF_INSTALL_PREFIX/bin"
    )

    # 检查头文件中的版本信息
    for path in "${bin_paths[@]}"; do
        local protoc_file="$path/protoc"
        if [ -f "$protoc_file" ]; then
            echo "发现已安装的 protobuf (protoc路径: $path)"
            return 0
        fi
    done

    # protobuf 目录
    cd "$GRPC_INSTALL_PREFIX/grpc/third_party/protobuf" || exit
    mkdir build
    cd build || exit

    # 配置、编译和安装
    echo "配置protobuf..."
    cmake -DCMAKE_INSTALL_PREFIX="$PROTOBUF_INSTALL_PREFIX" \
          -DABSL_PROPAGATE_CXX_STD=ON \
          -DCMAKE_BUILD_TYPE=$GRPC_BUILD_TYPE \
          -Dprotobuf_BUILD_SHARED_LIBS=ON \
          -Dprotobuf_BUILD_TESTS=OFF \
          ..

    echo "编译protobuf (使用 12 个线程)..."
    make -j12

    echo "安装protobuf..."
    sudo make install
    sudo ldconfig 2>/dev/null  # 刷新共享库缓存

    # 验证protobuf安装
    if command -v protoc &> /dev/null; then
        local new_version=$(protoc --version)
        echo "protobuf v${new_version} 安装成功"
        return
    fi

#    echo "protobuf安装失败"
#    exit 1
}

# 构建和安装gRPC
  build_and_install_grpc() {
    echo "开始构建gRPC..."
    cd "$GRPC_INSTALL_PREFIX/grpc" || exit
    mkdir -p cmake/build
    cd cmake/build || exit

    cmake -DCMAKE_BUILD_TYPE=$GRPC_BUILD_TYPE \
          -DCMAKE_INSTALL_PREFIX="$GRPC_INSTALL_PREFIX" \
          -DgRPC_INSTALL=ON \
          -DgRPC_BUILD_TESTS=OFF \
          -Dprotobuf_DIR="$PROTOBUF_INSTALL_PREFIX/" \
          -DgRPC_ZLIB_PROVIDER=package \
          -DgRPC_CARES_PROVIDER=package \
          -DgRPC_SSL_PROVIDER=package \
          ../..

    echo "编译gRPC (使用 12 个线程)..."
    make -j12

    echo "安装gRPC到 $GRPC_INSTALL_PREFIX..."
    sudo make install
    sudo ldconfig 2>/dev/null  # Linux系统刷新共享库缓存
}

# 验证安装
  grpc_verify_installation() {
    echo "验证gRPC安装..."

    local bin_paths=(
        "$GRPC_INSTALL_PREFIX/bin"
    )

    # 检查头文件中的版本信息
    for path in "${bin_paths[@]}"; do
        local grpc_cpp_plugin_file="$path/grpc_cpp_plugin"
        if [ -f "grpc_cpp_plugin_file" ]; then
            echo "发现已安装的 grpc_cpp_plugin (grpc_cpp_plugin路径: $path)"
            return 0
        fi
    done
    echo "gRPC 安装可能失败，请检查错误信息"
    exit 1
}

# 清理临时文件
cleanup() {
    read -p "是否删除grpc构建目录? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "正在清理构建文件..."
        rm -rf "$GRPC_INSTALL_PREFIX"
        echo "构建目录已删除"
    else
        echo "构建目录保留在: $GRPC_INSTALL_PREFIX"
    fi
}

install_grpc() {
  if check_existing_grpc 0; then
    echo "结束 grpc 安装"
    cd "$GRPC_SHELL_DIR" || exit
    return
  fi
  install_grpc_dependencies
  create_grpc_dir
  clone_grpc_repositories
  install_protobuf
  build_and_install_grpc
  grpc_verify_installation
  cd "$GRPC_SHELL_DIR" || exit
}