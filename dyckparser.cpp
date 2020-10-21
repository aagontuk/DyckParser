#include <iostream>
#include <iomanip>
#include <iterator>
#include <string>
#include <vector>
#include <algorithm>

int main (int argc, char *argv[]) {
    std::string input; 
    std::vector<int> scan_node;
    std::vector<int> scan_depth;
    std::vector<int> prefix_node;
    std::vector<int> prefix_depth;
    std::vector<int> depth;
    std::vector<int> bfs;
    std::vector<int> parent;
    std::vector<int> count;
    std::vector<int> nchild;
    std::vector<int> alloc;
    std::vector<int> output;

    /* input from stdin */
    std::cin >> input;

    /* scan input and create preliminary node and depth */
    for (int i = 0; i < input.length(); i++) {
        if (input[i] == '[') {
            scan_node.push_back(1);
            scan_depth.push_back(1);
        }
        else {
            scan_node.push_back(0);
            scan_depth.push_back(-1);
        }
    }

    /* prefix sum */
    prefix_node.push_back(0);
    prefix_depth.push_back(0);
    for (int i = 0; i < scan_node.size() - 1; i++) {
        prefix_node.push_back(scan_node[i] + prefix_node[i]); 
        prefix_depth.push_back(scan_depth[i] + prefix_depth[i]);
    }

    /* not balanced. warn and exit */
    if (prefix_depth[prefix_depth.size() - 1] != 1) {
        std::cout << "This is not a balanced string! Try again.\n"; 
        return -1;
    }

    /* set size and initialize arrays */
    int num_nodes = prefix_node[prefix_node.size() - 1];
    depth.resize(num_nodes);
    bfs.resize(num_nodes);
    parent.resize(num_nodes, 0);
    count.resize(num_nodes, 0);
    nchild.resize(num_nodes, 0);
    alloc.resize(num_nodes, 0);
    
    /* reduce to nodes */
    for (int i = 0; i < prefix_depth.size() - 1; i++) {
        depth[prefix_node[i]] = prefix_depth[i];
    }

    /* sort by depth */
    int index = 0;
    int max_depth = *std::max_element(std::begin(depth), std::end(depth));

    for (int i = 0; i <= max_depth; i++) {
        for (int j = 0; j < num_nodes; j++) {
            if (depth[j] == i) {
                bfs[j] = index++;
            } 
        }
    }

    /* calculate parent */
    for (int i = 0; i < num_nodes - 1; i++) {
        if (depth[i + 1] == (depth[i] + 1)) {
            parent[bfs[i + 1]] = bfs[i];
        } 
    }
    
    /* propagate to the right */
    int current_value = parent[0];
    for (int i = 0; i < num_nodes; i++) {
        if (parent[i] <= current_value) {
            parent[i] = current_value; 
        } 
        else {
            current_value = parent[i]; 
        }
    }

    /* calculate count and nchild array */
    for (int i = 0; i < num_nodes; i++) {
        if (parent[i] != parent[i + 1]) {
            count[i + 1] = 1; 
            nchild[parent[i]] = count[i];
        } 
        else {
            count[i + 1] = count[i] + 1; 
        }
    }

    /* special case only for parent with highest depth 1 */
    if (parent[parent.size() - 1] == 0) {
        nchild[0] = count[count.size() - 1]; 
    }

    /* calculate alloc */
    for (int i = 0; i < num_nodes; i++) {
        alloc[i + 1] = alloc[i] + nchild[i] + 1;
    }

    /* generate final output */
    int output_length = alloc[num_nodes - 1] + 1;
    output.resize(output_length);

    for (int i = 0; i < num_nodes; i++) {
        output[alloc[i]] = nchild[i]; 
    }
    
    /* generate references to children */
    int i = 0;
    int k = 0;
    while (i < output_length) {
        /* find non empty cell */ 
        if (output[i]) {
            for (int j = 1; j <= output[i]; j++) {
                output[i + j] = alloc[++k];
            }
            i += (output[i] + 1);
        }
        else {
            i++; 
        }
    }
   
    std::cout << "Index: ";
    for (int i = 0; i < output_length; i++) {
        std::cout << std::setw(2) << i << " "; 
    }
    std::cout << "\n";
    
    std::cout << "Value: ";
    for (auto elem : output) {
        std::cout << std::setw(2) << elem << " "; 
    }
    std::cout << "\n";
    
    return 0;
}
