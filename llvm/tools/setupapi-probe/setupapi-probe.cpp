// CI-only probe for PR 226794: runs the Setup API lookup alone.
#include "llvm/Support/VirtualFileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/WindowsDriver/MSVCPaths.h"
#include <optional>
#include <string>

int main() {
  std::string Path;
  llvm::ToolsetLayout Layout;
  auto VFS = llvm::vfs::getRealFileSystem();
  if (!llvm::findVCToolChainViaSetupConfig(*VFS, std::nullopt, Path, Layout)) {
    llvm::errs() << "Setup API: no Visual Studio found\n";
    return 1;
  }
  llvm::outs() << "Setup API: " << Path << " (VS2017OrNewer: "
               << (Layout == llvm::ToolsetLayout::VS2017OrNewer) << ")\n";
  return 0;
}
