#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Node {
    std::string name;
    bool is_dir;
    Node* parent;
    Node* first_child;
    Node* next_sibling;
    std::string full_path;

    Node(const std::string& node_name, bool dir, Node* node_parent)
        : name(node_name),
          is_dir(dir),
          parent(node_parent),
          first_child(nullptr),
          next_sibling(nullptr),
          full_path("") {}
};

static std::string trimSlashes(const std::string& input) {
    size_t start = 0;
    while (start < input.size() && input[start] == '/') {
        ++start;
    }
    size_t end = input.size();
    while (end > start && input[end - 1] == '/') {
        --end;
    }
    return input.substr(start, end - start);
}

static std::vector<std::string> splitPath(const std::string& path) {
    std::vector<std::string> parts;
    std::string trimmed = trimSlashes(path);
    std::string current;
    for (char ch : trimmed) {
        if (ch == '/') {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
        } else {
            current.push_back(ch);
        }
    }
    if (!current.empty()) {
        parts.push_back(current);
    }
    return parts;
}

static Node* findChild(Node* parent, const std::string& name) {
    for (Node* child = parent->first_child; child != nullptr;
         child = child->next_sibling) {
        if (child->name == name) {
            return child;
        }
    }
    return nullptr;
}

static Node* addChild(Node* parent, const std::string& name, bool is_dir) {
    Node* existing = findChild(parent, name);
    if (existing != nullptr) {
        if (is_dir) {
            existing->is_dir = true;
        }
        return existing;
    }

    Node* node = new Node(name, is_dir, parent);
    if (parent->first_child == nullptr) {
        parent->first_child = node;
    } else {
        Node* tail = parent->first_child;
        while (tail->next_sibling != nullptr) {
            tail = tail->next_sibling;
        }
        tail->next_sibling = node;
    }
    return node;
}

static Node* ensurePath(Node* root, const std::string& full_path) {
    Node* current = root;
    for (const std::string& part : splitPath(full_path)) {
        current = addChild(current, part, true);
    }
    current->is_dir = true;
    current->full_path = full_path;
    return current;
}

static std::string buildFullPath(const Node* node) {
    if (node == nullptr) {
        return "";
    }
    std::vector<std::string> parts;
    const Node* current = node;
    while (current != nullptr && !current->name.empty()) {
        parts.push_back(current->name);
        current = current->parent;
    }
    std::string path = "/";
    for (size_t i = 0; i < parts.size(); ++i) {
        path += parts[parts.size() - 1 - i];
        if (i + 1 < parts.size()) {
            path += "/";
        }
    }
    return path;
}

static void printTree(const Node* node, int depth) {
    if (node == nullptr) {
        return;
    }
    for (const Node* child = node->first_child; child != nullptr;
         child = child->next_sibling) {
        std::cout << std::string(depth * 5, ' ') << child->name << "\n";
        if (child->is_dir) {
            printTree(child, depth + 1);
        }
    }
}

static bool endsWithPath(const std::string& full_path,
                         const std::string& search_path) {
    std::string full_trim = trimSlashes(full_path);
    std::string search_trim = trimSlashes(search_path);
    if (search_trim.empty()) {
        return true;
    }
    if (full_trim == search_trim) {
        return true;
    }
    if (full_trim.size() <= search_trim.size()) {
        return false;
    }
    size_t pos = full_trim.size() - search_trim.size();
    if (full_trim.compare(pos, search_trim.size(), search_trim) != 0) {
        return false;
    }
    return full_trim[pos - 1] == '/';
}

static void deleteTree(Node* node) {
    if (node == nullptr) {
        return;
    }
    Node* child = node->first_child;
    while (child != nullptr) {
        Node* next = child->next_sibling;
        deleteTree(child);
        child = next;
    }
    delete node;
}

int main(int argc, char* argv[]) {
    std::ifstream input("LinuxDir.txt");
    if (!input) {
        std::cerr << "Error: LinuxDir.txt not found in current directory.\n";
        return 1;
    }

    Node* root = new Node("", true, nullptr);
    Node* home_root = nullptr;
    std::vector<Node*> directories;

    std::string line;
    Node* current_dir = nullptr;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            current_dir = nullptr;
            continue;
        }
        if (!line.empty() && line.back() == ':') {
            std::string dir_path = line.substr(0, line.size() - 1);
            current_dir = ensurePath(root, dir_path);
            directories.push_back(current_dir);
            if (dir_path == "/home") {
                home_root = current_dir;
            }
            continue;
        }
        if (current_dir != nullptr) {
            addChild(current_dir, line, false);
        }
    }

    if (argc <= 1) {
        Node* start = home_root != nullptr ? home_root : root;
        std::string root_path = buildFullPath(start);
        if (root_path.empty()) {
            root_path = "/";
        }
        std::cout << root_path << "/\n";
        printTree(start, 1);
        deleteTree(root);
        return 0;
    }

    std::string search_path = argv[1];
    bool any_match = false;
    for (const Node* dir : directories) {
        if (!dir->is_dir) {
            continue;
        }
        if (endsWithPath(dir->full_path, search_path)) {
            std::string header = buildFullPath(dir);
            if (header.empty()) {
                header = dir->full_path;
            }
            std::cout << header << "/\n";
            printTree(dir, 1);
            any_match = true;
        }
    }

    if (!any_match) {
        std::cout << "Directory not found.\n";
    }

    deleteTree(root);
    return 0;
}
