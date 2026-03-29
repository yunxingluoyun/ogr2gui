OGR2GUI 是一个开源项目，为 OGR2OGR 提供图形用户界面 （GUI）。OGR2OGR 是用于转换和操作地理空间数据的命令行实用程序。

将原项目进行升级：

1. 其在Qt5+GDAL3.7.2能够正常编译；
2. 增加ogr2gui_zh.ts支持简体中文；
3. 将工程改造为 CMake，统一 Qt5 / Qt6 与 GDAL 的构建入口；
4. 修复执行流程中 `append / update / SQL query` 未真正传入 OGR 核心逻辑的问题；
5. 补强错误提示与资源释放，便于继续维护和扩展。

## 构建说明

- macOS / Linux：确保系统可用 `cmake`、Qt、GDAL
- Windows：确保 `cmake` 能找到 Qt 与 GDAL，必要时设置 `CMAKE_PREFIX_PATH`、`GDAL_ROOT`

示例：

```bash
cmake -S . -B build
cmake --build build
```

当前仓库已在 `Qt 6.10.2 + GDAL 3.12.3` 环境下做过构建验证，同时保留对 `Qt5 + GDAL 3.7.2` 升级目标的兼容。

![](https://github.com/yunxingluoyun/ogr2gui/blob/master/images/%E7%95%8C%E9%9D%A2.png)

原项目地址：

[ogr2gui download | SourceForge.net](https://sourceforge.net/projects/ogr2gui/)

Copyright © 2009, Inventis


This file is part of ogr2gui.

Ogr2gui is free software; you can redistribute it and/or modify
it under the terms of version 3 of the GNU General Public License as published by
the Free Software Foundation.

Ogr2gui is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ogr2gui; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
