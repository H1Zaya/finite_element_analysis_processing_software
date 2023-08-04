#ifndef MYTRIANGLE_H
#define MYTRIANGLE_H

class myTriangle {
public:

    int getNode1() const;
    void setNode1(int node1);

    int getNode2() const;
    void setNode2(int node2);

    int getNode3() const;
    void setNode3(int node3);

    int getId() const;
    void setId(int id);


private:
    int m_node1;
    int m_node2;
    int m_node3;
    int m_id;
};

#endif // MYTRIANGLE_H
