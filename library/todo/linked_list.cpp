#include<bits/stdc++.h>
using namespace std;
#define rep(i,N) for(i=0;i<N;i++)
#define ll long long

using data_type = ll;

/* 双方向連結リスト */
class linked_list{
private:
    struct node{
        data_type data;
        ll index;
        node *next,*prev;
    };
    map<ll,node*> index_map;
    node *head,*tail;
    ll size;
public:
    linked_list(){
        head = new node;
        tail = new node;
        head->next = tail;
        tail->prev = head;
        size = 0;
    }
    /* indexの後ろに連結 */
    void insert(ll index,data_type data){
        node *new_node = new node;
        new_node->data = data;
        new_node->index = index;
        new_node->next = tail;
        new_node->prev = tail->prev;
        tail->prev->next = new_node;
        tail->prev = new_node;
        index_map[index] = new_node;
        size++;
    }
    void erase(ll index){
        node *current = index_map[index];
        current->prev->next = current->next;
        current->next->prev = current->prev;
        index_map.erase(index);
        delete current;
        size--;
    }
    data_type get(ll index){
        node *current = head->next;
        while(current != tail){
            if(current->index == index){
                return current->data;
            }
            current = current->next;
        }
        return -1;
    }
    ll get_size(){
        return size;
    }
    void print(){
        node *current = head->next;
        while(current != tail){
            cout << current->data << " ";
            current = current->next;
        }
        cout << endl;
    }
};

int main(void){
    ll N;

    ll i,j,k;



    return 0;
}