#pragma once

#define MAX_ADJ 30
#define MAX_JNT 20
#define MAX_TAG 16
#define MAX_NAMELEN 20
#define MAX_NODES 100

#include "ofMain.h"
#include <iostream>
#include <string>

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	class Node;
	class Edge;

	class Node {
	public:
		int x, y;
		int adj_count;
		string name;
		Edge* connected[MAX_ADJ];
		Node* drawnext;
		stack<Edge*> path; //path ã����
		int totalweight; //path ã����
		bool travelable; //path ã����
		//path ã���� ����ϴ� ���� : shortest path ���� ������ �׻� �ʱ�ȭ�Ұ�
	};

	class Edge {
	public:
		int tag;
		int weight;
		glm::vec2 joint[MAX_JNT + 2]; //���̴� ��(����, �� ����)
		int jointcount;
		Node* points[2];
		Edge* drawnext;
	};
	
	Node* Nodehead;
	Edge* Edgehead;

	int nodeRadius = 12;
	int lineWidth = 10;
	int nodeCount = 0;
	ofTrueTypeFont font;

	bool edgeFlag; // true : create edge when node created, �ٸ� �뼱 ������ false�� �ʱ�ȭ
	bool isDrawing; // drawing mode�� true
	bool isFinding; //find path ����� true

	ofColor colorSet[MAX_TAG] = { ofColor(0x00,0x52,0xa4),ofColor(0x00,0xa8,0x4d),
	ofColor(0xef,0x7c,0x1c),ofColor(0x00,0xa5,0xde),
	ofColor(0x99,0x6c,0xac),ofColor(0xcd,0x7c,0x2f),
	ofColor(0x74,0x7f,0x00),ofColor(0xe6,0x18,0x6c),
	ofColor(0xbd,0xb0,0x92),ofColor(0xf5,0xa2,0x00),
	ofColor(0x77,0xc4,0xa3),ofColor(0xd4,0x00,0x3b),
	ofColor(0x0c,0x8e,0x72),ofColor(0x00,0x90,0xd2),
	ofColor(0xb0,0xce,0x18),ofColor(0x67,0x89,0xca),};//1~9,���κд�,����,�źд�,����,��ö,���̽ż�,�Ÿ�

	int jointCount; // ���ο� edge ������ 0���� �ʱ�ȭ
	glm::vec2 tmpjoint[MAX_JNT];
	Node* selectedNode;
	Edge* selectedEdge;
	Node* making; //���� ����/�������� node�� ��Ÿ��
	int currentTag; //���� �������� edge�� tag (0���� ����)
	vector<Edge*> shortpath; //���� ª�� path

	void createNode(Node* nodehead, int mouse_x, int mouse_y);
	void createEdge(Edge* edgehead, Node* trail, Node* cur);
	void makeJoint(int mouse_x, int mouse_y);
	Node* selectNode(Node* nodehead, int mouse_x, int mouse_y);
	Edge* selectEdge(int mouse_x, int mouse_y);
	bool isidenticalNode(Edge* edgehead, Node* point1, Node* point2);
	void tagChange(Edge* edgehead);
	void setWeight();
	void setNodename();
	void initalizeSP(Node* nodehead);
	void shortestPath(Node* startnode);
	void findPath();
	void drawMap(Node* nodes, Edge* edges);
	void drawPreview(int mouse_x, int mouse_y);
	void drawPath();
	void drawInterface();
	void reset();
};
