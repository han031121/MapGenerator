#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	Nodehead = new Node;
	Edgehead = new Edge;
	Nodehead->drawnext = NULL;
	Edgehead->drawnext = NULL;
	edgeFlag = false;
	jointCount = 0;
	isDrawing = false;
	selectedNode = NULL;
	making = NULL;
	currentTag = 0;
	font.loadFont("verdana.ttf", 12, true, true);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(255, 255, 255);
	ofSetLineWidth(lineWidth);
	int x = ofGetMouseX();
	int y = ofGetMouseY();

	if (isDrawing)
		drawPreview(x, y);
	drawMap(Nodehead, Edgehead);
	if (!isDrawing)
		drawPath();
	drawInterface();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 's') {
		if (isDrawing) {
			isDrawing = false;
			selectedNode = NULL;
			selectedEdge = NULL;
			shortpath = {};
			printf("\n===Select Mode===\n");
		}
		else {
			isDrawing = true;
			printf("\n===Drawing Mode===\n");
		}
	}

	if (key == 'r') {
		reset();
	}

	if (isDrawing) { //in Drawing Mode
		if (key == 'd') {
			tagChange(Edgehead);
		}
	}

	if (!isDrawing && !isFinding) { //in Select Mode
		if (key == 'q') {
			if (selectedEdge)
				deleteEdge(selectedEdge);
			else if (selectedNode)
				deleteNode(selectedNode);
			else
				printf("you need to select node or edge first\n");
		}
		if (key == 'w') {
			setWeight();
		}
		if (key == 'e') {
			setNodename();
		}
	}

	if (!isDrawing) { //not in Drawing Mode
		if (key == 'f') {
			if (!isFinding)
				findPath();
			else
				isFinding = false;
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	if(isDrawing)
		selectedNode = selectNode(Nodehead, x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (isDrawing) {
		if (button == 0) {
			createNode(Nodehead, x, y);
		}
		else if (button == 2 && edgeFlag == true) {
			makeJoint(x, y);
		}
	}
	else {
		if (button == 0) {
			shortpath = {};
			if (isFinding) {
				findPath();
				selectedNode = NULL;
			}
			else if (selectedNode) {
				selectedEdge = selectEdge(x, y);
				selectedNode = NULL;
				if (selectedEdge) {
					return;
				}
			}
			selectedNode = selectNode(Nodehead, x, y);
			selectedEdge = NULL;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::createNode(Node* nodehead, int mouse_x, int mouse_y) {
	Node* cur = nodehead;
	Node* trail = NULL;

	if (making != NULL) {
		if (making->adj_count == MAX_ADJ) {
			printf("you can't make no more edges adjacent to this node (%d %d)\n", making->x, making->y);
			return;
		}
	}

	while (cur->drawnext != NULL)
		cur = cur->drawnext;

	if (selectedNode) {
		if (selectedNode->adj_count == MAX_ADJ) {
			printf("you can't make no more edges adjacent to this node (%d %d)\n", selectedNode->x, selectedNode->y);
			return;
		}
		trail = making;
		if (isidenticalNode(Edgehead, trail, selectedNode)) {
			printf("you can't make identical edges\n");
			return;
		}
		making = selectedNode;
		if (trail == making) //자기자신과 링크 불가
			return;

		if(trail!=NULL)
			printf("%d %d ~ %d %d linked\n", trail->x, trail->y, making->x, making->y);
		else
			printf("%d %d linked\n", making->x, making->y);
	}
	else if(selectNode(Nodehead, mouse_x, mouse_y) == NULL) {
		if (nodeCount == MAX_NODES) {
			printf("you can't make no more node\n");
			return;
		}
		if (cur != Nodehead && cur->adj_count == MAX_ADJ) {
			printf("you can't make no more edges adjacent to this node (%d %d)\n", cur->x, cur->y);
			return;
		}

		cur->drawnext = new Node;
		trail = making;
		cur = cur->drawnext;

		cur->name = "";
		cur->adj_count = 0;
		cur->x = mouse_x;
		cur->y = mouse_y;
		cur->drawnext = NULL;
		making = cur;
		nodeCount++;

		for (int i = 0; i < MAX_ADJ; i++)
			cur->connected[i] = NULL;

		printf("%d %d created\n", making->x, making->y);
	}
	else {
		return;
	}

	if (edgeFlag == true)
		createEdge(Edgehead, trail, making);
	else
		edgeFlag = true;
}

void ofApp::createEdge(Edge* edgehead, Node* trail, Node* cur) {
	Edge* edgecur = edgehead;

	while (edgecur->drawnext != NULL)
		edgecur = edgecur->drawnext;

	trail->adj_count++;
	cur->adj_count++;

	edgecur->drawnext = new Edge;
	edgecur = edgecur->drawnext;
	edgecur->tag = currentTag;
	edgecur->points[0] = trail;
	trail->connected[trail->adj_count-1] = edgecur;
	edgecur->points[1] = cur;
	cur->connected[cur->adj_count-1] = edgecur;
	edgecur->drawnext = NULL;
	edgecur->weight = 1;

	edgecur->joint[0] = glm::vec2(trail->x, trail->y);
	for (int i = 1; i <= jointCount; i++) {
		edgecur->joint[i] = tmpjoint[i-1];
	}
	edgecur->joint[jointCount+1] = glm::vec2(cur->x, cur->y);
	edgecur->jointcount = jointCount;
	jointCount = 0;
}

void ofApp::deleteNode(Node* delNode) {
	Node* prev = Nodehead;
	int adjcount = delNode->adj_count;

	while (prev->drawnext != delNode && prev->drawnext != NULL)
		prev = prev->drawnext;

	for (int i = 0; i < adjcount; i++)
		deleteEdge(delNode->connected[0]);

	prev->drawnext = delNode->drawnext;
	printf("%d %d deleted\n", delNode->x, delNode->y);
	free(delNode);

	edgeFlag = false;
	selectedNode = NULL;
	selectedEdge = NULL;
}

void ofApp::deleteEdge(Edge* delEdge) {
	Node* adj;
	Edge* prev = Edgehead;
	int adjcount;
	bool edgefound = false;

	while (prev->drawnext != delEdge && prev->drawnext != NULL)
		prev = prev->drawnext;

	for (int i = 0; i < 2; i++) {
		adj = delEdge->points[i];
		adjcount = adj->adj_count;

		for (int j = 0; j < adjcount; j++) {
			if (edgefound) {	
				adj->connected[j - 1] = adj->connected[j];
				adj->connected[j] = NULL;
			}
			else if (adj->connected[j] == delEdge) {
				edgefound = true;
				if (j == MAX_ADJ - 1)
					adj->connected[j] == NULL;
			}
		}
		adj->adj_count--;

		prev->drawnext = delEdge->drawnext;
	}
	printf("%d %d ~ %d %d deleted\n", delEdge->points[0]->x, delEdge->points[0]->y, 
		delEdge->points[1]->x, delEdge->points[1]->y);
	free(delEdge);

	selectedNode = NULL;
	selectedEdge = NULL;
}

void ofApp::makeJoint(int mouse_x,int mouse_y) {
	if (selectedNode) {
		making = selectedNode;
		jointCount = 0;
		return;
	}
	if (jointCount == MAX_JNT)
		return;
	tmpjoint[jointCount] = glm::vec2(mouse_x, mouse_y);
	jointCount++;
}

ofApp::Node* ofApp::selectNode(Node* nodehead, int mouse_x, int mouse_y) {
	glm::vec2 mouse = glm::vec2(mouse_x, mouse_y);
	float len;

	while (nodehead->drawnext != NULL) {
		nodehead = nodehead->drawnext;
		len = glm::distance(mouse, glm::vec2(nodehead->x, nodehead->y));

		if (len <= nodeRadius * 3) {
			return nodehead;
		}
	}
	return NULL;
}

ofApp::Edge* ofApp::selectEdge(int mouse_x, int mouse_y) {
	Node* newselect = selectNode(Nodehead, mouse_x, mouse_y);
	if (newselect == NULL)
		return NULL;

	for (int i = 0; i < selectedNode->adj_count; i++) {
		if (selectedNode->connected[i]->points[0] == newselect || selectedNode->connected[i]->points[1] == newselect) {
			return selectedNode->connected[i];
		}
	}
	selectedEdge = NULL;
}

void ofApp::tagChange(Edge* edgehead) {
	bool tagexist = false;
	Edge* tmp;
	currentTag++;
	if (currentTag == MAX_TAG)
		currentTag = 0;
	
	while (edgehead->drawnext != NULL) {
		edgehead = edgehead->drawnext;
		if (currentTag == edgehead->tag) {
			tagexist = true;
			tmp = edgehead;
		}
	}
	if (tagexist) {
		making = tmp->points[1];
		edgeFlag = true;
	}
	else {
		making = NULL;
		edgeFlag = false;
	}
	printf("tag changed (tag : %d)\n", currentTag);
}

void ofApp::setWeight() {
	if (!selectedEdge) {
		printf("you need to select an edge to set weight\n");
		return;
	}
	int x1 = selectedEdge->points[0]->x;
	int y1 = selectedEdge->points[0]->y;
	int x2 = selectedEdge->points[1]->x;
	int y2 = selectedEdge->points[1]->y;
	int tmp;

	while (1) {
		printf("weight of edge (%d %d ~ %d %d) : ", x1, y1, x2, y2);
		cin >> tmp;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(10, '\n');
			tmp = 0;
		}
		if(tmp>=1 && tmp<=100)
			break;
		printf("please enter valid value (1~100)\n");
	}

	printf("complete setting weight\n");
	selectedEdge->weight = tmp;
}

void ofApp::setNodename() {
	if (!selectedNode) {
		printf("you need to select a node to set name\n");
		return;
	}

	string tmp;

	while (1) {
		int x1 = selectedNode->x;
		int y1 = selectedNode->y;
		printf("enter node name in english (%d %d) : ", x1, y1);
		cin >> tmp;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(10, '\n');
			return;
		}
		if (tmp.size() < MAX_NAMELEN)
			break;
		printf("please enter valid value (under %d texts)\n", MAX_NAMELEN);
	}

	printf("complete setting name\n");
	selectedNode->name = tmp;
}

bool ofApp::isidenticalNode(Edge* edgehead, Node* point1, Node* point2) {
	while (edgehead->drawnext != NULL) {
		edgehead = edgehead->drawnext;
		if (currentTag == edgehead->tag) {
			if (edgehead->points[0] == point1 && edgehead->points[1] == point2)
				return true;
			if (edgehead->points[1] == point1 && edgehead->points[0] == point2)
				return true;
		}
	}
	return false;
}

void ofApp::initalizeSP(Node* nodehead) {
	while (nodehead->drawnext != NULL) {
		nodehead = nodehead->drawnext;
		nodehead->path = {};
		nodehead->totalweight = 9999999;
		nodehead->travelable = true;
	}
	shortpath = {};
}

void ofApp::shortestPath(Node* startnode) {
	initalizeSP(Nodehead);

	Node* cur = startnode;
	Node* adj;
	queue<Node*> search;

	search.push(startnode);
	startnode->totalweight = 0;

	while (!search.empty()) {
		cur = search.front();
		search.pop();
		cur->travelable = false;

		for (int i = 0; i < cur->adj_count; i++) {
		for (int j = 0; j < 2; j++) {
			adj = cur->connected[i]->points[j];

			if (adj == cur)
				continue;

			if (adj->totalweight > cur->totalweight + cur->connected[i]->weight) { //갱신
				adj->totalweight = cur->totalweight + cur->connected[i]->weight;
				adj->path = cur->path;
				adj->path.push(cur->connected[i]);
				adj->travelable = true;
			}
			if (adj->travelable == true)
				search.push(adj);
		}
		}
	}
}

void ofApp::findPath() {
	Node* dest = NULL;
	int x = ofGetMouseX();
	int y = ofGetMouseY();

	if (!isFinding) {
		if (!selectedNode) {
			printf("you need to select a node first\n");
			return;
		}
		printf("select another node to find path\n");
		shortestPath(selectedNode);
		isFinding = true;
	}
	else {
		dest = selectNode(Nodehead, x, y);
		if (dest) {
			while (!dest->path.empty()) {
				shortpath.push_back(dest->path.top());
				dest->path.pop();
			}
		}
		if (shortpath.empty())
			printf("there is no path\n");
		else
			printf("path found\n");
		isFinding = false;
	}
}

void ofApp::drawMap(Node* nodes, Edge* edges) {
	while (edges->drawnext != NULL) {
		edges = edges->drawnext;
		for (int i = 1; i < edges->jointcount + 2; i++) {
			if (selectedEdge == edges)
				ofSetLineWidth(lineWidth * 1.8);
			else
				ofSetLineWidth(lineWidth);
			ofSetColor(colorSet[edges->tag]);
			ofDrawLine(edges->joint[i - 1], edges->joint[i]);

			if (selectedEdge == edges) {
				ofDrawCircle(edges->joint[i - 1], lineWidth * 1.8 / 2);
			}
			else {
				ofDrawCircle(edges->joint[i - 1], lineWidth / 2);
			}
		}
	}
	while (nodes->drawnext != NULL) {
		nodes = nodes->drawnext;
		if (nodes==selectedNode)
			ofSetColor(255, 0, 0);
		else
			ofSetColor(0, 0, 0);
		ofDrawCircle(nodes->x, nodes->y, nodeRadius);
		ofSetColor(255, 255, 255);
		ofDrawCircle(nodes->x, nodes->y, nodeRadius/2);
	}
}

void ofApp::drawPreview(int mouse_x,int mouse_y) {
	/*draw Node preview*/
	ofSetColor(0, 0, 0, 80);
	ofDrawCircle(mouse_x, mouse_y, nodeRadius);

	/*draw Edge preview*/
	if (edgeFlag == false)
		return;

	ofSetColor(colorSet[currentTag], 80);

	if (jointCount > 0) {
		ofDrawLine(glm::vec2(making->x, making->y), tmpjoint[0]);
		for (int i = 1; i < jointCount; i++) {
			ofDrawLine(tmpjoint[i-1], tmpjoint[i]);
		}
		ofDrawLine(tmpjoint[jointCount-1], glm::vec2(mouse_x, mouse_y));
	}
	else {
		ofDrawLine(making->x, making->y, mouse_x, mouse_y);
	}
}

void ofApp::drawPath() {
	for (int j = 0; j < shortpath.size(); j++) {
		for (int i = 1; i < shortpath[j]->jointcount + 2; i++) {
			ofSetLineWidth(lineWidth * 1.8);
			ofSetColor(colorSet[shortpath[j]->tag]);
			ofDrawLine(shortpath[j]->joint[i - 1], shortpath[j]->joint[i]);
			ofDrawCircle(shortpath[j]->joint[i - 1], lineWidth * 1.8 / 2);
		}
	}

	for (int i = 0; i < shortpath.size(); i++) {
	for (int j = 0; j < 2; j++) {
		ofSetColor(255, 0, 0);
		ofDrawCircle(shortpath[i]->points[j]->x, shortpath[i]->points[j]->y, nodeRadius);
		ofSetColor(255, 255, 255);
		ofDrawCircle(shortpath[i]->points[j]->x, shortpath[i]->points[j]->y, nodeRadius / 2);
	}
	}
}

void ofApp::drawInterface() {
	Node* curnode = Nodehead;
	Edge* curedge = Edgehead;
	int edgex, edgey;
	int x = ofGetMouseX();
	int y = ofGetMouseY();

	ofSetColor(96, 96, 96);

	while (curnode->drawnext != NULL) {
		curnode = curnode->drawnext;
		if (curnode->name.size() > 0) {
			font.drawString(curnode->name, curnode->x, curnode->y + 25);
			//이름 쓰기
		}
	}

	while (curedge->drawnext != NULL) {
		curedge = curedge->drawnext;
		edgex = (curedge->points[0]->x + curedge->points[1]->x) / 2;
		edgey = (curedge->points[0]->y + curedge->points[1]->y) / 2;
		font.drawString(to_string(curedge->weight), edgex, edgey);
		//weight 쓰기
	}

	if (isDrawing) {
		font.drawString("Drawing Mode", 3, 24);
		font.drawString("current tag : " + to_string(currentTag), 30, 51);
		ofSetColor(colorSet[currentTag]);
		ofDrawRectangle(3, 27, 24, 24);
		ofSetColor(96, 96, 96);
		font.drawString(to_string(x) + " " + to_string(y), 3, 78);
	}
	else {
		if (isFinding || !shortpath.empty())
			font.drawString("Path Find Mode", 3, 24);
		else
			font.drawString("Select Mode", 3, 24);
		if (selectedNode) {
			font.drawString(to_string(selectedNode->x) + " " + to_string(selectedNode->y), 3, 51);
		}
	}
}

void ofApp::reset() {
	Edge* Edgedel;
	Edge* Edgetmp = Edgehead;
	Node* Nodedel;
	Node* Nodetmp = Nodehead;
	while (Edgetmp != NULL) {
		Edgedel = Edgetmp;
		Edgetmp = Edgedel->drawnext;
		delete(Edgedel);
	}
	while (Nodetmp != NULL) {
		Nodedel = Nodetmp;
		Nodetmp = Nodedel->drawnext;
		delete(Nodedel);
	}
	setup();

	printf("\n===Reset all status===\n");
}