/* 22. Составить   программу  поиска  записи  с  включением  в 
сильно ветвящемся  Б-дереве  порядка N.  Б-дерево  хранится  и
обрабатывается в оперативной памяти (12).
Рычков Александр 
ПС-23

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <map>

using namespace std;

struct vertexNode {
    int key;
    string value;
};

struct BTree {
    vector<vertexNode> node;
    vector<BTree*> connection;
    BTree* parent; 
    int level;
   
};


BTree* root = nullptr;
int n = 0; 
map<int, vector<BTree*>> levelNodes;


BTree* createBTreeNode(int level, BTree* parent = nullptr) {
    BTree* newNode = new BTree;
    newNode->level = level;
    
    newNode->parent = parent; 
    return newNode;
}


int findInsertPosition(BTree* node, int key) {
    int i = 0;
    while (i < node->node.size() && key > node->node[i].key) {
        i++;
    }
    return i;
}


void splitChild(BTree* parentNode, int childIndex) {
    BTree* childNode = parentNode->connection[childIndex];
    BTree* newChildNode = createBTreeNode(childNode->level);

    int medianIndex = n - 1;

    parentNode->node.insert(parentNode->node.begin() + childIndex, childNode->node[medianIndex]);
    parentNode->connection.insert(parentNode->connection.begin() + childIndex + 1, newChildNode);

    newChildNode->node.assign(childNode->node.begin() + n, childNode->node.end());

    if (!childNode->connection.empty()) {
        newChildNode->connection.assign(childNode->connection.begin() + n, childNode->connection.end());
        childNode->connection.resize(n);
    }
    childNode->node.resize(medianIndex);
}


void insertNonFull(BTree* node, int key, const string& value)
{
    int i = node->node.size() - 1;

    if (node->connection.empty())
    {
        vertexNode newVertexNode = { key, value };

        int insertPos = findInsertPosition(node, key);
        node->node.insert(node->node.begin() + insertPos, newVertexNode);

    }
    else {
        while (i >= 0 && key < node->node[i].key)
        {
            i--;
        }
        i++;

        if (node->connection[i]->node.size() == 2 * n - 1)
        {
            splitChild(node, i);
            if (key > node->node[i].key)
            {
                i++;
            }
        }
        insertNonFull(node->connection[i], key, value);
    }
}


void destroyTree(BTree* node)
{
    if (node) {
        for (BTree* child : node->connection)
        {
            destroyTree(child);
        }
        delete node;
    }
}




void printNode(BTree* node)
{
    if (node == nullptr) return;

    for (int i = 0; i < node->level; ++i)
    {
        cout << ".";
    }
    for (size_t i = 0; i < node->node.size(); ++i)
    {
        cout << node->node[i].key << " " << node->node[i].value;
        if (i < node->node.size() - 1)
        {
            cout << ", ";
        }
    }
    cout << endl;

    for (BTree* child : node->connection)
    {
        printNode(child);
    }

}


void OutputBTree(BTree* node)
{
    if (node != nullptr)
    {
        printNode(node);
    }
}

bool keySearchInBTree(BTree* node, int key, bool& flagkey) {
    if (node == nullptr) {
        flagkey = false;
        return false;
    }

    for (const auto& pair : node->node) {
        if (pair.key == key) {
            cout << "Ключ " << key << " -- значение: " << pair.value << endl;
            return true;
        }
    }

    int i = 0;
    while (i < node->node.size() && key > node->node[i].key) {
        i++;
    }

    if (i < node->connection.size()) {
        return keySearchInBTree(node->connection[i], key, flagkey);
    }

    flagkey = false;
    cout << "Ключ " << key << " не существует" << endl;
    return false;
}

void incrementLevels(BTree* node) {
    if (node == nullptr) {
        return;
    }

    node->level++;
    

    for (BTree* child : node->connection) {
        incrementLevels(child);
    }
}



void AddNode(BTree* node, int key, const string& value) {
    
    if (node->node.empty()) {
        vertexNode newVertexNode = { key, value };
        node->node.push_back(newVertexNode);
        return;
    }

    if (node->connection.empty()) {
        int insertPos = findInsertPosition(node, key);
        vertexNode newVertexNode = { key, value };
        node->node.insert(node->node.begin() + insertPos, newVertexNode);
    }
    else {
        int i = findInsertPosition(node, key);
        AddNode(node->connection[i], key, value);
    }

    if (node->node.size() > 2 * n - 1) {
        int medianIdx = node->node.size() / 2;
        vertexNode medianNode = node->node[medianIdx];

        if (node == root) {
            BTree* newRoot = createBTreeNode(0, 0);
            BTree* leftChild = createBTreeNode(1, newRoot);
            BTree* rightChild = createBTreeNode(1, newRoot);
            leftChild->node.assign(node->node.begin(), node->node.begin() + medianIdx);
            rightChild->node.assign(node->node.begin() + medianIdx + 1, node->node.end());

            if (!node->connection.empty()) {
                leftChild->connection.assign(node->connection.begin(),
                    node->connection.begin() + medianIdx + 1);
                rightChild->connection.assign(node->connection.begin() + medianIdx + 1,
                    node->connection.end());

                for (auto& child : leftChild->connection) {
                    child->parent = leftChild;
                    child->level++;
                    for (auto& grandChild : child->connection) {
                        if (grandChild != nullptr) {
                            incrementLevels(grandChild);
                        }
                    }
                }
                for (auto& child : rightChild->connection) {
                    child->parent = rightChild;
                    child->level++;
                    for (auto& grandChild : child->connection) {
                        if (grandChild != nullptr) {
                            incrementLevels(grandChild);
                        }
                    }
                }
            }

            newRoot->node.push_back(medianNode);
            newRoot->connection.push_back(leftChild);
            newRoot->connection.push_back(rightChild);

            root = newRoot;
            levelNodes.clear();
            levelNodes[0].push_back(newRoot);
            levelNodes[1].push_back(leftChild);
            levelNodes[1].push_back(rightChild);
        }
        else {
            BTree* newNode = createBTreeNode(node->level, node->parent);
            newNode->node.assign(node->node.begin() + medianIdx + 1, node->node.end());

            if (!node->connection.empty()) {
                newNode->connection.assign(node->connection.begin() + medianIdx + 1,
                    node->connection.end());
                node->connection.resize(medianIdx + 1);

                for (auto& child : newNode->connection) {
                    child->parent = newNode;
                }
            }

            node->node.resize(medianIdx);

            BTree* parent = node->parent;
            int insertPos = findInsertPosition(parent, medianNode.key);
            parent->node.insert(parent->node.begin() + insertPos, medianNode);
            parent->connection.insert(parent->connection.begin() + insertPos + 1, newNode);
        }
    }
}

void FromFileBTree(const string& filename)
{
    ifstream inputFile(filename);
    if (!inputFile.is_open())
    {
        cerr << "Ошибка открытия файла: " << filename << endl;
        return;
    }

    string line;

    if (!getline(inputFile, line))
    {
        cerr << "Файл пуст или не содержит значение n." << endl;
        inputFile.close();
        return;
    }

    stringstream ss(line);
    ss >> n;
    if (n <= 0)
    {
        cerr << "Некорректное значение n: " << n << endl;
        inputFile.close();
        return;
    }

    bool isFirstPair = true; 

    while (getline(inputFile, line))
    {
        if (line.empty()) continue; 

        stringstream ssData(line);
        int key;
        string value;

        if (!(ssData >> key >> value))
        {
            cerr << "Некорректный формат строки: " << line << endl;
            continue; 
        }

        if (isFirstPair)
        {
            root = createBTreeNode(0, nullptr);
            vertexNode newVertexNode = { key, value };
            root->node.push_back(newVertexNode);
            isFirstPair = false;
        }
        else
        {
            AddNode(root, key, value);
        }
    }

    inputFile.close();
}

void writeToFile(const string& filename, int key, const string& value) {
    ofstream outputFile(filename, ios::app);
    if (!outputFile.is_open()) {
        cerr << "Ошибка открытия файла для записи: " << filename << endl;
        return;
    }
    outputFile << endl << key << " " << value;
    outputFile.close();
}

int main() {
    setlocale(LC_ALL, "Russian");
    int n = 0;
    string filename;
    cout << "Введите имя входного файла:";
    cin >> filename;

    FromFileBTree(filename);

    cout << "\nБ-ДЕРЕВО:" << endl;
    OutputBTree(root);
    bool flag = true;
    bool flagkey = true;
    int Key;
    string val;
    BTree* head = root;
    while (flag)
    {
        cout << "\nВведите ключ для поиска ";
        cin >> Key;
        
        if (keySearchInBTree(head, Key, flagkey) == false)
        {
            int num;
            

            cout << "Желаете вставить ключ?"<< endl<< "1)ДА" << endl << "2)НЕТ"<< endl<< "3)Закончить программу" << endl;
            cin >> num;
            if (num == 1)
            {
                cout << "Введите значение для ключа ";
                cin >> val;


                if (head == nullptr) {
                head = createBTreeNode(0, 0);
                AddNode(head, Key, val);
                levelNodes[0].push_back(head);
                }
                else {
                AddNode(head, Key, val);
                }
                OutputBTree(root);
                head = root;
                writeToFile(filename, Key, val);
            }
            else if (num == 3)
            {
                flag = false;
            }
            
        }
    }
    
   // destroyTree(root);

    return 0;
}