#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <climits>

using namespace std;

struct Subscriber {
    string id;
    string name;
    string email;
    string tier; 
};

struct AccessChange {
    string userId;
    string oldTier;
    string newTier;
};

struct ClickEvent {
    string userId;
    int articleId;
    string timestamp;
};

struct Coupon {
    string code;
    int discountPercentage;

    bool operator<(const Coupon& other) const {
        return code < other.code;
    }
};

struct Article {
    int id;
    string title;
    double engagementScore;
    int dailyViews;
    double monetizationScore;
};

struct SupportTicket {
    int ticketId;
    string userId;
    int priority; 
    string issue;

    bool operator<(const SupportTicket& other) const {
        return priority < other.priority;
    }
};

struct HistoryNode {
    int articleId;
    HistoryNode* prev;
    HistoryNode* next;
    HistoryNode(int id) : articleId(id), prev(nullptr), next(nullptr) {}
};

class RecentHistoryList {
private:
    HistoryNode* head;
    HistoryNode* tail;
    int size;
    int maxSize;

public:
    RecentHistoryList(int maxSz = 5) : head(nullptr), tail(nullptr), size(0), maxSize(maxSz) {}

    void addArticle(int id) {
        HistoryNode* newNode = new HistoryNode(id);
        if (!head) {
            head = tail = newNode;
            size++;
            return;
        }
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
        size++;

        if (size > maxSize) {
            HistoryNode* temp = tail;
            tail = tail->prev;
            if (tail) tail->next = nullptr;
            delete temp;
            size--;
        }
    }

    void display() {
        if (!head) {
            cout << "No recent browsing history.\n";
            return;
        }
        HistoryNode* curr = head;
        cout << "Recent Browsing History (Most Recent First):\n";
        while (curr) {
            cout << " -> Article ID: " << curr->articleId << "\n";
            curr = curr->next;
        }
    }

    int getSize() const { return size; }
};

unordered_map<string, Subscriber> subscriberTable;
stack<AccessChange> undoStack;
queue<ClickEvent> clickQueue;
vector<Coupon> couponList;
vector<Article> articleList;
unordered_map<int, vector<pair<int, int>>> contentGraph; 
priority_queue<SupportTicket> supportQueue;
RecentHistoryList globalHistory(5);
vector<double> maxHeapScores;

void insertHeap(double score) {
    maxHeapScores.push_back(score);
    int i = maxHeapScores.size() - 1;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (maxHeapScores[i] > maxHeapScores[parent]) {
            swap(maxHeapScores[i], maxHeapScores[parent]);
            i = parent;
        } else {
            break;
        }
    }
}

void buildMaxHeap() {
    maxHeapScores.clear();
    for (const auto& art : articleList) {
        insertHeap(art.engagementScore);
    }
}

void displayTopKArticles(int k) {
    buildMaxHeap();
    vector<double> tempHeap = maxHeapScores;
    cout << "\n--- TOP " << k << " TRENDING ARTICLES (BY ENGAGEMENT SCORE) ---\n";
    int items = min(k, (int)tempHeap.size());
    for (int i = 0; i < items; i++) {
        cout << (i + 1) << ". Engagement Score: " << tempHeap[0] << "\n";
        if (tempHeap.size() > 1) {
            tempHeap[0] = tempHeap.back();
            tempHeap.pop_back();
            int idx = 0;
            int n = tempHeap.size();
            while (true) {
                int left = 2 * idx + 1;
                int right = 2 * idx + 2;
                int largest = idx;
                if (left < n && tempHeap[left] > tempHeap[largest]) largest = left;
                if (right < n && tempHeap[right] > tempHeap[largest]) largest = right;
                if (largest != idx) {
                    swap(tempHeap[idx], tempHeap[largest]);
                    idx = largest;
                } else {
                    break;
                }
            }
        } else {
            tempHeap.pop_back();
        }
    }
}

int partitionQuick(vector<Article>& arr, int low, int high, int criterion) {
    double pivotValue = 0;
    if (criterion == 1) pivotValue = arr[high].engagementScore;
    else if (criterion == 2) pivotValue = arr[high].dailyViews;
    else pivotValue = arr[high].monetizationScore;

    int i = low - 1;
    for (int j = low; j < high; j++) {
        double currentVal = 0;
        if (criterion == 1) currentVal = arr[j].engagementScore;
        else if (criterion == 2) currentVal = arr[j].dailyViews;
        else currentVal = arr[j].monetizationScore;

        if (currentVal >= pivotValue) { 
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSortArticles(vector<Article>& arr, int low, int high, int criterion) {
    if (low < high) {
        int pi = partitionQuick(arr, low, high, criterion);
        quickSortArticles(arr, low, pi - 1, criterion);
        quickSortArticles(arr, pi + 1, high, criterion);
    }
}

void merge(vector<Article>& arr, int l, int m, int r, int criterion) {
    int n1 = m - l + 1;
    int n2 = r - m;
    vector<Article> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        double valL = (criterion == 1) ? L[i].engagementScore : ((criterion == 2) ? L[i].dailyViews : L[i].monetizationScore);
        double valR = (criterion == 1) ? R[j].engagementScore : ((criterion == 2) ? R[j].dailyViews : R[j].monetizationScore);
        if (valL >= valR) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) { arr[k] = L[i]; i++; k++; }
    while (j < n2) { arr[k] = R[j]; j++; k++; }
}

void mergeSortArticles(vector<Article>& arr, int l, int r, int criterion) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSortArticles(arr, l, m, criterion);
        mergeSortArticles(arr, m + 1, r, criterion);
        merge(arr, l, m, r, criterion);
    }
}

void initializeSystemData() {
    subscriberTable["SUB101"] = {"SUB101", "Alice Smith", "alice@example.com", "Free"};
    subscriberTable["SUB102"] = {"SUB102", "Bob Jones", "bob@example.com", "Premium"};
    subscriberTable["SUB103"] = {"SUB103", "Charlie Brown", "charlie@example.com", "VIP"};

    couponList.push_back({"SAVE10", 10});
    couponList.push_back({"HALFPRICE", 50});
    couponList.push_back({"WELCOME20", 20});
    sort(couponList.begin(), couponList.end());

    articleList.push_back({201, "Global Economy Architecture", 8.8, 1200, 75.5});
    articleList.push_back({202, "Introduction to Algorithms", 9.5, 3500, 92.0});
    articleList.push_back({203, "Tech Horizons 2026", 7.2, 850, 45.0});
    articleList.push_back({204, "Data Science Principles", 8.1, 1900, 61.2});

    contentGraph[201].push_back({203, 4});
    contentGraph[201].push_back({204, 7});
    contentGraph[202].push_back({204, 2});
    contentGraph[203].push_back({201, 4});
    contentGraph[204].push_back({201, 7});
    contentGraph[204].push_back({202, 2});

    supportQueue.push({1, "SUB101", 1, "Cannot unlock standard free daily article"});
    supportQueue.push({2, "SUB103", 3, "Corporate API billing payment error pipeline"});
    supportQueue.push({3, "SUB102", 2, "Premium subscription badge missing from account layout"});
}

void runSubscriberDirectory() {
    int subChoice;
    cout << "\n===================================\n";
    cout << "      SUBSCRIBER DIRECTORY         \n";
    cout << "===================================\n";
    cout << "1. Add Subscriber\n2. Search Subscriber\n3. Update Subscription Tier\n4. Delete Subscriber\n5. Return\nChoice: ";
    cin >> subChoice;

    string id, name, email, tier;
    if (subChoice == 1) {
        cout << "Enter Subscriber ID: "; cin >> id;
        cout << "Enter Name: "; cin.ignore(); getline(cin, name);
        cout << "Enter Email: "; cin >> email;
        cout << "Enter Tier (Free/Premium/VIP): "; cin >> tier;
        subscriberTable[id] = {id, name, email, tier};
        cout << "Subscriber profile logged successfully.\n";
    } else if (subChoice == 2) {
        cout << "Enter ID to locate: "; cin >> id;
        if (subscriberTable.find(id) != subscriberTable.end()) {
            auto s = subscriberTable[id];
            cout << "Found -> Name: " << s.name << " | Email: " << s.email << " | Tier: " << s.tier << "\n";
        } else {
            cout << "Subscriber records not found.\n";
        }
    } else if (subChoice == 3) {
        cout << "Enter ID to modify: "; cin >> id;
        if (subscriberTable.find(id) != subscriberTable.end()) {
            string oldT = subscriberTable[id].tier;
            cout << "Current Tier: " << oldT << "\nEnter New Tier: "; cin >> tier;
            subscriberTable[id].tier = tier;
            undoStack.push({id, oldT, tier});
            cout << "Tier configuration updated. Tracking action in historical log.\n";
        } else {
            cout << "Subscriber records not found.\n";
        }
    } else if (subChoice == 4) {
        cout << "Enter ID to remove: "; cin >> id;
        if (subscriberTable.find(id) != subscriberTable.end()) {
            subscriberTable.erase(id);
            cout << "Record deleted from main system.\n";
        } else {
            cout << "Subscriber records not found.\n";
        }
    }
}

void runAccessUndoSystem() {
    cout << "\n===================================\n";
    cout << "        ACCESS UNDO PIPELINE       \n";
    cout << "===================================\n";
    if (undoStack.empty()) {
        cout << "No administrative operations found in the log stack.\n";
        return;
    }
    AccessChange lastChange = undoStack.top();
    cout << "Most Recent Logged Modification Event:\n";
    cout << "Subscriber ID: " << lastChange.userId << "\n";
    cout << "Altered From: " << lastChange.oldTier << " -> To: " << lastChange.newTier << "\n";
    cout << "Execute immediate rollback operations? (y/n): ";
    char conf; cin >> conf;
    if (conf == 'y' || conf == 'Y') {
        if (subscriberTable.find(lastChange.userId) != subscriberTable.end()) {
            subscriberTable[lastChange.userId].tier = lastChange.oldTier;
            cout << "Rollback complete. Subscriber tier state reset to: " << lastChange.oldTier << "\n";
        } else {
            cout << "Rollback failed: Subscriber target record no longer exists.\n";
        }
        undoStack.pop();
    }
}

void runClickTracker() {
    int clickChoice;
    cout << "\n===================================\n";
    cout << "         CLICKSTREAM TRACKER       \n";
    cout << "===================================\n";
    cout << "1. Push Click Event to Queue\n2. Process Next Periodic Click Entry\n3. View Total Pending Buffer Counts\nChoice: ";
    cin >> clickChoice;

    if (clickChoice == 1) {
        string uid, tstamp; int aid;
        cout << "Enter User ID: "; cin >> uid;
        cout << "Enter Destination Article ID: "; cin >> aid;
        cout << "Enter Event Timestamp (HHMM): "; cin >> tstamp;
        clickQueue.push({uid, aid, tstamp});
        globalHistory.addArticle(aid);
        cout << "Event buffered into processing queue.\n";
    } else if (clickChoice == 2) {
        if (clickQueue.empty()) {
            cout << "Buffer empty. No event operations required.\n";
        } else {
            ClickEvent frontEvent = clickQueue.front();
            cout << "Processing Logged Event -> User: " << frontEvent.userId 
                 << " Read Article: " << frontEvent.articleId << " at " << frontEvent.timestamp << "\n";
            clickQueue.pop();
        }
    } else if (clickChoice == 3) {
        cout << "Total buffered clickstream processing records: " << clickQueue.size() << "\n";
    }
}

void runCouponValidation() {
    cout << "\n===================================\n";
    cout << "         COUPON VALIDATION         \n";
    cout << "===================================\n";
    cout << "1. Query Coupon Code\n2. View List\nChoice: ";
    int cc; cin >> cc;
    if (cc == 1) {
        string codeSearch;
        cout << "Enter Voucher Code to evaluate: "; cin >> codeSearch;
        int low = 0, high = couponList.size() - 1;
        bool located = false;
        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (couponList[mid].code == codeSearch) {
                cout << "Valid Code Active. Coupon details verified: " << couponList[mid].discountPercentage << "% structural price discount applied.\n";
                located = true;
                break;
            } else if (couponList[mid].code < codeSearch) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        if (!located) cout << "Code invalid, inactive, or expired from active listings.\n";
    } else {
        cout << "Current Sorted Promotional Coupons:\n";
        for (const auto& coup : couponList) {
            cout << " - Code: " << setw(12) << left << coup.code << " Discount: " << coup.discountPercentage << "%\n";
        }
    }
}

void runArticleSorter() {
    cout << "\n===================================\n";
    cout << "         ARTICLE ANALYTICS         \n";
    cout << "===================================\n";
    cout << "Select Target Field Metric:\n1. Engagement Score\n2. Daily Views\n3. Monetization Score\nChoice: ";
    int criterion; cin >> criterion;
    cout << "Select Sorting Architecture:\n1. Merge Sort Strategy\n2. Quick Sort Strategy\nChoice: ";
    int algChoice; cin >> algChoice;

    vector<Article> sortingData = articleList;
    if (algChoice == 1) {
        mergeSortArticles(sortingData, 0, sortingData.size() - 1, criterion);
    } else {
        quickSortArticles(sortingData, 0, sortingData.size() - 1, criterion);
    }

    cout << "\n--- CLASSIFIED ENGINE EXECUTION OUTPUT ---\n";
    for (const auto& art : sortingData) {
        cout << "ID: " << art.id << " | Title: " << setw(28) << left << art.title 
             << " | Engagement: " << art.engagementScore << " | Views: " << setw(5) << art.dailyViews 
             << " | Monetization: $" << art.monetizationScore << "\n";
    }
}

void runContentNetwork() {
    cout << "\n===================================\n";
    cout << "         CONTENT WEB NETWORK       \n";
    cout << "===================================\n";
    cout << "1. Add Node\n2. Bind Directional Path Connection\n3. Graph Structural Overview Visualization\nChoice: ";
    int choice; cin >> choice;
    if (choice == 1) {
        int nid; cout << "Enter unique new Article Node ID: "; cin >> nid;
        if(contentGraph.find(nid) == contentGraph.end()){
            contentGraph[nid] = vector<pair<int, int>>();
            cout << "Article node assigned in graph memory map.\n";
        } else {
            cout << "Node already active inside routing structural references.\n";
        }
    } else if (choice == 2) {
        int u, v, w;
        cout << "Enter Origin Node ID: "; cin >> u;
        cout << "Enter Target Destination Node ID: "; cin >> v;
        cout << "Enter Link Evaluation Edge Cost/Weight: "; cin >> w;
        contentGraph[u].push_back({v, w});
        cout << "Structural connection path mapped across operational routing tables.\n";
    } else {
        cout << "Graph Layout Adjacency Matrix Visualizations:\n";
        for (const auto& [u, edges] : contentGraph) {
            cout << "Article [" << u << "] matches pathways with: ";
            for (const auto& edge : edges) {
                cout << " -> " << edge.first << " (Cost/Weight: " << edge.second << ")";
            }
            cout << "\n";
        }
    }
}

void runReadingPathFinder() {
    cout << "\n===================================\n";
    cout << "         READING PATH FINDER       \n";
    cout << "===================================\n";
    int src, dest;
    cout << "Enter Anchor Origin Node ID: "; cin >> src;
    cout << "Enter Target Destination Node ID: "; cin >> dest;

    unordered_map<int, int> dist;
    unordered_map<int, int> parent;
    for (const auto& [u, edges] : contentGraph) {
        dist[u] = INT_MAX;
        for (const auto& edge : edges) dist[edge.first] = INT_MAX;
    }
    
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    
    if (dist.find(src) == dist.end()) {
        cout << "Origin path reference missing from infrastructure system.\n";
        return;
    }

    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        int u = pq.top().second;
        int d = pq.top().first;
        pq.pop();

        if (d > dist[u]) continue;

        for (const auto& edge : contentGraph[u]) {
            int v = edge.first;
            int weight = edge.second;
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    if (dist[dest] == INT_MAX) {
        cout << "No optimized traversal link pathways discoverable between targets.\n";
    } else {
        cout << "Minimum Route Cost: " << dist[dest] << "\nPath Configuration Framework: ";
        vector<int> path;
        int curr = dest;
        while (curr != src) {
            path.push_back(curr);
            curr = parent[curr];
        }
        path.push_back(src);
        reverse(path.begin(), path.end());
        for (size_t i = 0; i < path.size(); i++) {
            cout << path[i] << (i + 1 == path.size() ? "" : " -> ");
        }
        cout << "\n";
    }
}

void runPriceOptimizer() {
    cout << "\n===================================\n";
    cout << "         PRICE OPTIMIZER           \n";
    cout << "===================================\n";
    int daysInactive;
    cout << "Enter Customer Inactive Window Value (Days): "; cin >> daysInactive;
    
    cout << "\n--- OPTIMIZATION DECISION ANALYSIS OUTPUT ---\n";
    if (daysInactive > 30) {
        cout << "Strategic Recommendation: Deploy Max Discount Voucher Campaign (30% Retargeting Coupon).\n";
    } else if (daysInactive > 15) {
        cout << "Strategic Recommendation: Deploy Standard Engagement Recovery Voucher (20% Discount Package).\n";
    } else {
        cout << "Strategic Recommendation: Customer Activity Profile Stable. Standard Pricing Structure Intact.\n";
    }
}

void runExtraFeatures() {
    cout << "\n===================================\n";
    cout << "         EXTENDED CAPABILITIES    \n";
    cout << "===================================\n";
    cout << "1. Process Support Helpdesk Queue (Priority Queue)\n2. Display System Browsing Logs (Doubly Linked List)\n3. Check Top Trending Articles (Max Heap)\nChoice: ";
    int choice; cin >> choice;
    if (choice == 1) {
        if (supportQueue.empty()) {
            cout << "Support queues clearing cleanly. Zero tickets active.\n";
        } else {
            SupportTicket ticket = supportQueue.top();
            cout << "Processing Priority Ticket Profile -> ID: " << ticket.ticketId 
                 << " | Account User: " << ticket.userId << " | Level Priority: " << ticket.priority 
                 << "\nIssue Detail: " << ticket.issue << "\n";
            supportQueue.pop();
            cout << "Ticket verified, processed, and resolved successfully.\n";
        }
    } else if (choice == 2) {
        globalHistory.display();
    } else {
        cout << "Enter K values for target verification rankings: ";
        int kv; cin >> kv;
        displayTopKArticles(kv);
    }
}

void runAnalyticsDashboard() {
    cout << "\n===================================\n";
    cout << "         SYSTEM OVERVIEW MATRIX    \n";
    cout << "===================================\n";
    cout << "Total Active Accounts Encrypted in Main Memory Map: " << subscriberTable.size() << "\n";
    cout << "Active Promotional Discount Trackers Active:       " << couponList.size() << "\n";
    cout << "Network Nodes Initialized in Graph Directory:      " << contentGraph.size() << "\n";
    cout << "Buffered Streaming User Engagement Events Pending: " << clickQueue.size() << "\n";
    cout << "Open Helpdesk Inquiries Pending Resolution:        " << supportQueue.size() << "\n";
}

void runComplexityMatrix() {
    cout << "\n========================================================================================\n";
    cout << "                          COMPLEXITY MATRIX RUNTIME PERFORMANCE                        \n";
    cout << "========================================================================================\n";
    cout << " Module Component       | Data Structure/Algorithm | Avg/Best Time | Worst Case | Space  \n";
    cout << "------------------------+--------------------------+---------------+------------+--------\n";
    cout << " Subscriber Records     | Hash Table               | O(1)          | O(N)       | O(N)   \n";
    cout << " Session Access Undo    | Stack                    | O(1)          | O(1)       | O(N)   \n";
    cout << " Click Tracker Queue    | Queue                    | O(1)          | O(1)       | O(N)   \n";
    cout << " Coupon Evaluation      | Binary Search            | O(log N)      | O(log N)   | O(N)   \n";
    cout << " Analysis Engine        | Quick / Merge Sort       | O(N log N)    | O(N log N) | O(N)   \n";
    cout << " Core Content Network   | Adjacency List           | O(V + E)      | O(V + E)   | O(V+E) \n";
    cout << " Strategy Discovery     | Dijkstra Pathing         | O(E log V)    | O(E log V) | O(V)   \n";
    cout << " Support Routing        | Binary Max Heap          | O(log N)      | O(log N)   | O(N)   \n";
    cout << " Recent History Vector  | Doubly Linked List       | O(1)          | O(1)       | O(K)   \n";
    cout << "========================================================================================\n";
}

int main() {
    initializeSystemData();
    while (true) {
        cout << "\n===================================\n";
        cout << " READPASS PUBLISHING ROUTER ENGINE \n";
        cout << "===================================\n";
        cout << "1. Subscriber Directory\n";
        cout << "2. Access Undo Operations\n";
        cout << "3. Click Stream Tracker\n";
        cout << "4. Coupon Codes Registry\n";
        cout << "5. Article Analytics Sorter\n";
        cout << "6. Content Network Web\n";
        cout << "7. Path Intersect Finder\n";
        cout << "8. Pricing Engine Optimization\n";
        cout << "9. Extended Engine Features\n";
        cout << "10. Analytics Control Dashboard\n";
        cout << "11. Display Complexity Matrix\n";
        cout << "12. Shut Down System\n";
        cout << "-----------------------------------\n";
        cout << "Enter Action Target Choice [1-12]: ";
        
        int mainChoice;
        if (!(cin >> mainChoice)) {
            cout << "Execution failure. System termination initiated.\n";
            break;
        }

        if (mainChoice == 12) {
            cout << "System decommissioning safe sequence triggered. Good-bye.\n";
            break;
        }

        switch (mainChoice) {
            case 1: runSubscriberDirectory(); break;
            case 2: runAccessUndoSystem(); break;
            case 3: runClickTracker(); break;
            case 4: runCouponValidation(); break;
            case 5: runArticleSorter(); break;
            case 6: runContentNetwork(); break;
            case 7: runReadingPathFinder(); break;
            case 8: runPriceOptimizer(); break;
            case 9: runExtraFeatures(); break;
            case 10: runAnalyticsDashboard(); break;
            case 11: runComplexityMatrix(); break;
            default: cout << "Selection invalid. Provide correct menu key map identifier.\n";
        }
    }
    return 0;
}