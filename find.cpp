#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct Node {
  std::string name;
  std::string fullPath;
  bool isDir;
  bool listed;
  Node* parent;
  Node* firstChild;
  Node* nextSibling;

  Node(std::string nameIn, std::string pathIn, bool dir)
      : name(std::move(nameIn)),
        fullPath(std::move(pathIn)),
        isDir(dir),
        listed(false),
        parent(nullptr),
        firstChild(nullptr),
        nextSibling(nullptr) {}
};

std::string NormalizePath(std::string path) {
  while (path.size() > 1 && path.back() == '/') {
    path.pop_back();
  }
  return path;
}

std::string BaseName(const std::string& path) {
  std::size_t lastSlash = path.find_last_of('/');
  if (lastSlash == std::string::npos) {
    return path;
  }
  return path.substr(lastSlash + 1);
}

std::string ParentPath(const std::string& path) {
  std::size_t lastSlash = path.find_last_of('/');
  if (lastSlash == std::string::npos || lastSlash == 0) {
    return "";
  }
  return path.substr(0, lastSlash);
}

std::string JoinPath(const std::string& base, const std::string& name) {
  if (base.empty()) {
    return name;
  }
  if (base == "/") {
    return "/" + name;
  }
  return base + "/" + name;
}

class DirectoryTree {
 public:
  Node* GetOrCreate(const std::string& path, bool isDir) {
    auto it = nodes.find(path);
    if (it != nodes.end()) {
      if (isDir) {
        it->second->isDir = true;
      }
      return it->second;
    }
    std::string name = BaseName(path);
    auto node = std::make_unique<Node>(name, path, isDir);
    Node* raw = node.get();
    nodes[path] = raw;
    storage.push_back(std::move(node));
    return raw;
  }

  void MarkListed(Node* node) {
    if (!node->listed) {
      node->listed = true;
      directoryOrder.push_back(node);
    }
  }

  void AddChild(Node* parent, Node* child) {
    if (!parent || !child) {
      return;
    }
    if (child->parent != nullptr) {
      return;
    }
    child->parent = parent;
    if (!parent->firstChild) {
      parent->firstChild = child;
      return;
    }
    Node* current = parent->firstChild;
    while (current->nextSibling) {
      current = current->nextSibling;
    }
    current->nextSibling = child;
  }

  std::vector<Node*> Roots() const {
    std::vector<Node*> roots;
    for (Node* node : directoryOrder) {
      if (node->parent == nullptr) {
        roots.push_back(node);
      }
    }
    return roots;
  }

  std::vector<Node*> FindMatches(const std::string& query) const {
    std::vector<Node*> matches;
    bool isAbsolute = !query.empty() && query[0] == '/';
    bool hasSlash = query.find('/') != std::string::npos;
    for (Node* node : directoryOrder) {
      if (!node->isDir) {
        continue;
      }
      if (isAbsolute) {
        if (node->fullPath == query) {
          matches.push_back(node);
        }
        continue;
      }
      if (!hasSlash) {
        if (node->name == query) {
          matches.push_back(node);
        }
        continue;
      }
      const std::string& fullPath = node->fullPath;
      if (fullPath.size() < query.size()) {
        continue;
      }
      if (fullPath.compare(fullPath.size() - query.size(), query.size(), query) != 0) {
        continue;
      }
      if (fullPath.size() == query.size() ||
          fullPath[fullPath.size() - query.size() - 1] == '/') {
        matches.push_back(node);
      }
    }
    return matches;
  }

 private:
  std::unordered_map<std::string, Node*> nodes;
  std::vector<std::unique_ptr<Node>> storage;
  std::vector<Node*> directoryOrder;
};

void PrintTree(const Node* node, int depth) {
  std::cout << std::string(depth * 5, ' ') << node->name << "\n";
  if (!node->isDir) {
    return;
  }
  const Node* child = node->firstChild;
  while (child) {
    PrintTree(child, depth + 1);
    child = child->nextSibling;
  }
}

void PrintListing(const Node* node) {
  std::cout << node->fullPath;
  if (node->fullPath.empty() || node->fullPath.back() != '/') {
    std::cout << '/';
  }
  std::cout << "\n";
  PrintTree(node, 0);
}

bool LoadDirectoryTree(std::istream& input, DirectoryTree& tree) {
  std::string line;
  Node* currentDir = nullptr;
  std::string currentPath;

  while (std::getline(input, line)) {
    if (line.empty()) {
      currentDir = nullptr;
      currentPath.clear();
      continue;
    }
    if (!line.empty() && line.back() == ':') {
      std::string dirPath = NormalizePath(line.substr(0, line.size() - 1));
      if (dirPath.empty()) {
        currentDir = nullptr;
        currentPath.clear();
        continue;
      }
      currentPath = dirPath;
      currentDir = tree.GetOrCreate(dirPath, true);
      tree.MarkListed(currentDir);
      continue;
    }
    if (!currentDir) {
      continue;
    }
    std::string childPath = JoinPath(currentPath, line);
    Node* child = tree.GetOrCreate(childPath, false);
    tree.AddChild(currentDir, child);
  }
  return true;
}

std::string NormalizeQuery(std::string query) {
  if (query.rfind("./", 0) == 0) {
    query.erase(0, 2);
  }
  return NormalizePath(std::move(query));
}

int main(int argc, char* argv[]) {
  std::ifstream input("LinuxDir.txt");
  if (!input) {
    input.open("linuxDir");
  }
  if (!input) {
    std::cerr << "Unable to open LinuxDir.txt in current directory.\n";
    return 1;
  }

  DirectoryTree tree;
  LoadDirectoryTree(input, tree);

  if (argc < 2) {
    std::vector<Node*> roots = tree.Roots();
    if (roots.empty()) {
      return 0;
    }
    for (Node* node : roots) {
      PrintListing(node);
    }
    return 0;
  }

  std::string query = NormalizeQuery(argv[1]);
  if (query.empty()) {
    std::vector<Node*> roots = tree.Roots();
    for (Node* node : roots) {
      PrintListing(node);
    }
    return 0;
  }

  std::vector<Node*> matches = tree.FindMatches(query);
  if (matches.empty()) {
    std::cerr << "Directory not found.\n";
    return 0;
  }
  for (Node* node : matches) {
    PrintListing(node);
  }
  return 0;
}
