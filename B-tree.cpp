#include <iostream>
#include <cstring>
#include <list>
const int T = 20;
using namespace std;

class VALUE
{
public:
    string word;
    char part_of_speech;
    float frequency;
    bool operator<(const VALUE value)
    {
        return word < value.word;
    }
    bool operator>(const VALUE value)
    {
        return word > value.word;
    }
    friend istream &operator>>(istream &is, VALUE &value)
    {
        is >> value.word >> value.part_of_speech >> value.frequency;
        return is;
    }
    friend ostream &operator<<(ostream &os, VALUE &value)
    {
        os << value.word + " " << value.part_of_speech << " " << value.frequency;
        return os;
    }
};

class B_tree
{
private:
    class B_TreeNode
    {
    public:
        int key_size;
        VALUE entries[2 * T - 1];
        B_TreeNode *child[2 * T];
        bool leaf;
        B_TreeNode()
        {
            key_size = 0;
            leaf = true;
        }
    };
    class LOCATION
    {
    public:
        B_TreeNode *node;
        int number;
        LOCATION(B_TreeNode *node, int number)
        {
            this->node = node;
            this->number = number;
        }
    };
    B_TreeNode *root;
    void split_child(B_TreeNode *x, int i) //将节点x第i个儿子进行分裂操作
    {
        B_TreeNode *z = new B_TreeNode;
        B_TreeNode *y = x->child[i];
        z->leaf = y->leaf;
        z->key_size = T - 1;
        for (int j = 0; j < T - 1; j++)
        {
            z->entries[j] = y->entries[j + T];
        }
        if (!y->leaf)
            for (int j = 0; j < T; j++)
            {
                z->child[j] = y->child[j + T];
            }
        y->key_size = T - 1;
        for (int j = x->key_size; j >= i + 1; j--)
        {
            x->child[j + 1] = x->child[j];
        }
        x->child[i + 1] = z;
        for (int j = x->key_size - 1; j >= i; j--)
        {
            x->entries[j + 1] = x->entries[j];
        }
        x->entries[i] = y->entries[T - 1];
        x->key_size++;
    }
    void insert_notfull(B_TreeNode *x, VALUE value)
    {
        int i = x->key_size - 1;
        if (x->leaf)
        {
            while (i >= 0 && value < x->entries[i])
            {
                x->entries[i + 1] = x->entries[i];
                i--;
            }
            x->entries[i + 1] = value;
            x->key_size++;
        }
        else
        {
            while (i >= 0 && value < x->entries[i])
            {
                i--;
            }
            i++;
            if (x->child[i]->key_size == 2 * T - 1)
            {
                split_child(x, i);
                if (value > x->entries[i])
                    i++;
            }
            insert_notfull(x->child[i], value);
        }
    }
    void delete_nonone(B_TreeNode *x, string k)
    {
        int i = 0;
        if (x->leaf)
        {
            while (i < x->key_size && k > x->entries[i].word)
            {
                i++;
            }
            if (k == x->entries[i].word)
            {
                for (int j = i + 1; j < x->key_size; j++)
                {
                    x->entries[j - 1] = x->entries[j];
                }
                x->key_size--;
            }
        }
        else
        {
            while (i < x->key_size && k > x->entries[i].word)
            {
                i++;
            }
            B_TreeNode *y = x->child[i], *z;
            if (i < x->key_size)
                z = x->child[i + 1];
            if (i<x->key_size && k == x->entries[i].word) // case2
            {
                if (y->key_size > T - 1) //case2(a)
                {
                    VALUE k_1 = find_predecessor(y);
                    delete_nonone(y, k_1.word);
                    x->entries[i] = k_1;
                }
                else if (z->key_size > T - 1)
                {
                    VALUE k_1 = find_successor(z);
                    delete_nonone(z, k_1.word);
                    x->entries[i] = k_1;
                }
                else
                {
                    merge(x, i, y, z);
                    delete_nonone(y, k);
                }
            }
            else     //case3
            {
                B_TreeNode *p;
                if (i > 0)
                    p = x->child[i - 1];
                if (y->key_size == T-1)
                {
                    if (i > 0 && p->key_size > T - 1)
                        shift_to_right_child(x, i-1, p, y);
                    else if (i < x->key_size && z->key_size > T - 1)
                    {
                        shift_to_left_child(x, i, y, z);
                    }
                    else if (i > 0)
                    {
                        merge(x, i-1, p, y);
                        y = p;
                    }
                    else
                        merge(x, i, y, z);
                }
                delete_nonone(y, k);
            }
        }
    }
    void shift_to_right_child(B_TreeNode *x, int i, B_TreeNode *y, B_TreeNode *z)
    {
        z->key_size++;
        int j = z->key_size - 1;
        while (j > 0)
        {
            z->entries[j] = z->entries[j - 1];
            j--;
        }
        z->entries[0] = x->entries[i];
        x->entries[i] = y->entries[y->key_size - 1];
        if (!z->leaf)
        {
            j = z->key_size - 1;
            while (j >= 0)
            {
                z->child[j + 1] = z->child[j];
                j--;
            }
            z->child[0] = y->child[y->key_size];
        }
        y->key_size--;
    }
    void shift_to_left_child(B_TreeNode *x, int i, B_TreeNode *y, B_TreeNode *z)
    {
        y->key_size++;
        y->entries[y->key_size - 1] = x->entries[i];
        x->entries[i] = z->entries[0];
        z->key_size--;
        int j = 0;
        while (j < z->key_size)
        {
            z->entries[j] = z->entries[j + 1];
            j++;
        }
        if (!z->leaf)
        {
            y->child[y->key_size] = z->child[0];
            j = 1;
            while (j <= z->key_size)
            {
                z->child[j] = z->child[j + 1];
                j++;
            }
        }
    }
    void merge(B_TreeNode *x, int i, B_TreeNode *y, B_TreeNode *z)
    {
        y->key_size = 2 * T - 1;
        for (int j = T; j < 2 * T - 1; j++)
        {
            y->entries[j] = z->entries[j - T];
        }
        y->entries[T - 1] = x->entries[i];
        if (!y->leaf)
        {
            for (int j = T; j < 2 * T; j++)
            {
                y->child[j] = z->child[j - T];
            }
        }
        for (int j = i + 1; j < x->key_size; j++)
        {
            x->child[j] = x->child[j + 1];
        }
        for (int j = i; j < x->key_size - 1; j++)
        {
            x->entries[j] = x->entries[j + 1];
        }
        x->key_size--;
    }
    VALUE find_successor(B_TreeNode *z) //查找后继
    {
        B_TreeNode *u = z;
        while (!u->leaf)
        {
            u = u->child[0];
        }
        return u->entries[0];
    }
    VALUE find_predecessor(B_TreeNode *y) //查找前驱
    {
        B_TreeNode *u = y;
        int i = u->key_size;
        while (!u->leaf)
        {
            u = u->child[i];
            i = u->key_size;
        }
        return u->entries[i - 1];
    }
    LOCATION *search(B_TreeNode *x, const string &key)
    {
        int i = 0;
        while (i < x->key_size && key > x->entries[i].word)
        {
            i++;
        }
        if (i < x->key_size && key == x->entries[i].word)
            return new LOCATION(x, i);
        else if (x->leaf)
        {
            return NULL;
        }
        else
            return search(x->child[i], key);
    }
    void delete_by_command(B_TreeNode *x, string key)
    {
        if (search(root, key) == NULL)
        {
            cout << "key " + key + " missing" << endl;
            return;
        }
        else
        {
            B_TreeNode *r = root, *y, *z;
            if (r->key_size == 1)
            {
                y = r->child[0];
                z = r->child[1];
                if (y->key_size == T - 1 && z->key_size == T - 1)
                {
                    merge(r, 0, y, z);
                    root = y;
                    delete_nonone(y, key);
                }
                else
                    delete_nonone(r, key);
            }
            else
                delete_nonone(r, key);
        }
    }
    void Dump(B_TreeNode *x)
    {
        if (x->leaf)
        {
            for (int i = 0; i < x->key_size; i++)
            {
                cout << x->entries[i] << endl;
            }
        }
        else
        {
            for (int i = 0; i < x->key_size; i++)
            {
                Dump(x->child[i]);
                cout << x->entries[i] << endl;
            }
            Dump(x->child[x->key_size]);
        }
    }
public:
    B_tree(/* args */)
    {
        freopen("project1\\init.txt", "r", stdin);
        root = new B_TreeNode();
        root->leaf = true;
        root->key_size = 0;
        string str;
        getline(cin, str);
        VALUE value;
        while (cin >> value)
        {
            insert_by_command(value);
        }
        fclose(stdin);
    }
    void search(const string key)
    {
        LOCATION *location = search(root, key);
        if (location == NULL)
        {
            cout << "key " + key + " missing" << endl;
        }
        else
        {
            cout << location->node->entries[location->number] << endl;
            delete location;
        }
    }
    void insert_by_command(VALUE value)
    {
        LOCATION *location = search(root, value.word);
        if (location != NULL)
        {
            cout << "key " + value.word + " conflict" << endl;
            delete location;
            return;
        }
        else
        {
            B_TreeNode *r = root;
            if (r->key_size == 2 * T - 1)
            {
                B_TreeNode *s = new B_TreeNode();
                root = s;
                s->leaf = false;
                s->key_size = 0;
                s->child[0] = r;
                split_child(s, 0);
                insert_notfull(s, value);
            }
            else
                insert_notfull(r, value);
        }
    }
    void insert_by_file(string filepath = "project1\\insert.txt")
    {
        freopen(filepath.c_str(), "r", stdin);
        string str;
        getline(cin, str);
        VALUE value;
        while (cin >> value)
        {
            insert_by_command(value);
        }
        fclose(stdin);
    }
    void delete_by_command(string key)
    {
        delete_by_command(root, key);
    }
    void delete_by_file(string filepath = "project1\\delete.txt")
    {
        freopen(filepath.c_str(), "r", stdin);
        string str;
        getline(cin, str);
        VALUE value;
        while (cin >> value)
        {
            delete_by_command(root, value.word);
        }
        fclose(stdin);
    }
    void Dump()
    {
        Dump(root);
    }
    void update(VALUE value)
    {
        if (search(root, value.word) == NULL)
        {
            insert_by_command(value);
        }
        else
        {
            LOCATION *location = search(root, value.word);
            location->node->entries[location->number] = value;
            delete location;
        }
    }
};
int main(int argc, char const *argv[])
{
    B_tree Tree;
    Tree.insert_by_file();
    Tree.delete_by_file();
    Tree.search("anti-indian");
    Tree.search("123");
    VALUE value;
    value.word="123";value.part_of_speech='a';value.frequency=12;
    Tree.insert_by_command(value);
    Tree.search("123");
    value.word="321";value.part_of_speech='a';value.frequency=12;
    Tree.search("321");
    Tree.update(value);
    Tree.search("321");
    Tree.delete_by_command("321");
    Tree.delete_by_command("321");
    Tree.Dump();
    return 0;
}
