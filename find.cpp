#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Node {
    std::string name;
    std::string path;
    bool is_dir;
    Node* first_child;
    Node* next_sibling;

    Node(const std::string& node_name,
         const std::string& node_path,
         bool dir)
        : name(node_name),
          path(node_path),
          is_dir(dir),
          first_child(nullptr),
          next_sibling(nullptr) {}
};

static std::string strip_slashes(const std::string& text) {
    size_t start = 0;
    while (start < text.size() && text[start] == '/') {
        ++start;
    }
    size_t end = text.size();
    while (end > start && text[end - 1] == '/') {
        --end;
    }
    return text.substr(start, end - start);
}

static std::vector<std::string> split_path(const std::string& path) {
    std::vector<std::string> parts;
    std::string cleaned = strip_slashes(path);
    std::string chunk;
    for (char ch : cleaned) {
        if (ch == '/') {
            if (!chunk.empty()) {
                parts.push_back(chunk);
                chunk.clear();
            }
        } else {
            chunk.push_back(ch);
        }
    }
    if (!chunk.empty()) {
        parts.push_back(chunk);
    }
    return parts;
}

static Node* find_child(Node* parent, const std::string& name) {
    for (Node* child = parent->first_child; child != nullptr;
         child = child->next_sibling) {
        if (child->name == name) {
            return child;
        }
    }
    return nullptr;
}

static Node* add_child(Node* parent, const std::string& name, bool is_dir) {
    Node* existing = find_child(parent, name);
    if (existing != nullptr) {
        if (is_dir) {
            existing->is_dir = true;
        }
        return existing;
    }

    std::string child_path = parent->path.empty()
                                 ? "/" + name
                                 : parent->path + "/" + name;
    Node* node = new Node(name, child_path, is_dir);
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

static Node* ensure_dir(Node* root, const std::string& path) {
    Node* current = root;
    for (const std::string& part : split_path(path)) {
        current = add_child(current, part, true);
    }
    current->is_dir = true;
    return current;
}

static void print_tree(const Node* node, int depth) {
    if (node == nullptr) {
        return;
    }
    for (const Node* child = node->first_child; child != nullptr;
         child = child->next_sibling) {
        std::cout << std::string(depth * 5, ' ') << child->name << "\n";
        if (child->is_dir) {
            print_tree(child, depth + 1);
        }
    }
}

static void print_directory(const Node* dir) {
    std::string header = dir->path.empty() ? "/" : dir->path;
    std::cout << header << "/\n";
    print_tree(dir, 1);
}

static bool path_matches(const std::string& full_path,
                         const std::string& search_path) {
    std::string full = strip_slashes(full_path);
    std::string search = strip_slashes(search_path);
    if (search.empty()) {
        return true;
    }
    if (full == search) {
        return true;
    }
    if (full.size() <= search.size()) {
        return false;
    }
    size_t pos = full.size() - search.size();
    return full.compare(pos, search.size(), search) == 0 && full[pos - 1] == '/';
}

static void destroy_tree(Node* node) {
    if (node == nullptr) {
        return;
    }
    Node* child = node->first_child;
    while (child != nullptr) {
        Node* next = child->next_sibling;
        destroy_tree(child);
        child = next;
    }
    delete node;
}

static void read_listing(std::istream& input,
                         Node* root,
                         std::vector<Node*>& directories,
                         Node*& home_root) {
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
        if (line.back() == ':') {
            std::string dir_path = line.substr(0, line.size() - 1);
            current_dir = ensure_dir(root, dir_path);
            directories.push_back(current_dir);
            if (dir_path == "/home") {
                home_root = current_dir;
            }
            continue;
        }
        if (current_dir != nullptr) {
            add_child(current_dir, line, false);
        }
    }
}

int main(int argc, char* argv[]) {
    std::ifstream input("LinuxDir.txt");
    if (!input) {
        std::cerr << "Error: LinuxDir.txt not found in current directory.\n";
        return 1;
    }

    Node* root = new Node("", "", true);
    Node* home_root = nullptr;
    std::vector<Node*> directories;

    read_listing(input, root, directories, home_root);

    if (argc <= 1) {
        Node* start = home_root;
        if (start == nullptr && root->first_child != nullptr &&
            root->first_child->next_sibling == nullptr) {
            start = root->first_child;
        }
        if (start == nullptr) {
            start = root;
        }
        print_directory(start);
        destroy_tree(root);
        return 0;
    }

    std::string search_path = argv[1];
    bool found = false;

    for (const Node* dir : directories) {
        if (dir->is_dir && path_matches(dir->path, search_path)) {
            print_directory(dir);
            found = true;
        }
    }

    if (!found) {
        std::cout << "Directory not found.\n";
    }

    destroy_tree(root);
    return 0;
}
