#include <stdlib.h>

#include "global.h"
#include "debug.h"

/**
 * @brief  Read genetic distance data and initialize data structures.
 * @details  This function reads genetic distance data from a specified
 * input stream, parses and validates it, and initializes internal data
 * structures.
 *
 * The input format is a simplified version of Comma Separated Values
 * (CSV).  Each line consists of text characters, terminated by a newline.
 * Lines that start with '#' are considered comments and are ignored.
 * Each non-comment line consists of a nonempty sequence of data fields;
 * each field iis terminated either by ',' or else newline for the last
 * field on a line.  The constant INPUT_MAX specifies the maximum number
 * of data characters that may be in an input field; fields with more than
 * that many characters are regarded as invalid input and cause an error
 * return.  The first field of the first data line is empty;
 * the subsequent fields on that line specify names of "taxa", which comprise
 * the leaf nodes of a phylogenetic tree.  The total number N of taxa is
 * equal to the number of fields on the first data line, minus one (for the
 * blank first field).  Following the first data line are N additional lines.
 * Each of these lines has N+1 fields.  The first field is a taxon name,
 * which must match the name in the corresponding column of the first line.
 * The subsequent fields are numeric fields that specify N "distances"
 * between this taxon and the others.  Any additional lines of input following
 * the last data line are ignored.  The distance data must form a symmetric
 * matrix (i.e. D[i][j] == D[j][i]) with zeroes on the main diagonal
 * (i.e. D[i][i] == 0).
 *
 * If 0 is returned, indicating data successfully read, then upon return
 * the following global variables and data structures have been set:
 *   num_taxa - set to the number N of taxa, determined from the first data line
 *   num_all_nodes - initialized to be equal to num_taxa
 *   num_active_nodes - initialized to be equal to num_taxa
 *   node_names - the first N entries contain the N taxa names, as C strings
 *   distances - initialized to an NxN matrix of distance values, where each
 *     row of the matrix contains the distance data from one of the data lines
 *   nodes - the "name" fields of the first N entries have been initialized
 *     with pointers to the corresponding taxa names stored in the node_names
 *     array.
 *   active_node_map - initialized to the identity mapping on [0..N);
 *     that is, active_node_map[i] == i for 0 <= i < N.
 *
 * @param in  The input stream from which to read the data.
 * @return 0 in case the data was successfully read, otherwise -1
 * if there was any error.  Premature termination of the input data,
 * failure of each line to have the same number of fields, and distance
 * fields that are not in numeric format should cause a one-line error
 * message to be printed to stderr and -1 to be returned.
 */

int read_distance_data(FILE *in) {
    char current_character;
    //character_count checks if the '#' symbol is the first character of the line.
    int character_count = 1;
    //checking current size of input length, checking if it exceeds MAX_INPUT length to return necessary error
    int input_length = 0;
    //counting number of valid lines
    int valid_line_count = 0;
    //char ** pointer to node names (char node_names[MAX_NODES][INPUT_MAX+1];)
    char *node_names_pointer = *(node_names + 0);
    //char * pointer to buffer
    char *buffer_pointer = (input_buffer + 0);
    //pointer to active_node_map
    int *active_node_map_pointer = (active_node_map + 0);
    //pointer to nodes array
    NODE *nodes_pointer = (nodes + 0);
    //number of fields in node_names
    int num_fields = 0;
    //num_row_nodes depicts the current count of nodes in the current row of the matrix
    int num_row_nodes = 0;
    //pointer to the distances matrix
    double *distances_pointer = *(distances);
    //end pointer for determining if the buffer string contains a valid double
    char *end_pointer;
    //value in double form of the current matrix input
    double value;
    //counts number of decimals in input field
    int dot_count = 0;

    while ((current_character = fgetc(in)) != EOF)
    {
        //checks if the current line is a comment, otherwise continues
        if (current_character == '#' && character_count == 1)
        {
            while ((current_character = fgetc(in) != '\n' && current_character != EOF))
            {
                //Skip line, as it's a comment so we ignore it entirely
            }
            character_count = 1;
        }
        else if (current_character == '\n' && valid_line_count > 1)
        {
            //checks for new line character, resets character count to 1 & terminates field
            character_count = 1;
            valid_line_count = valid_line_count + 1;
            num_row_nodes = 0;
        }
        else if (valid_line_count <= 1)
        {
            //updates character count for field and returns an error if it exceeds the INPUT_MAX 
            input_length = input_length + 1;
            if (input_length > INPUT_MAX && current_character != ',')
            {
                fprintf(stderr, "Error: Input field character length exceeds input max!\n");
                return -1;
            }
            //checks if this is the first character in a new line, therefore a new valid line
            if (character_count == 1)
            {
                valid_line_count = valid_line_count + 1;
            }
            if (current_character == ',' && character_count != 1)
            {
                input_length = 0;
                //end of current field, next character begins the next field
                //NULL terminating the current string field in node_names
                *buffer_pointer = '\0';
                if (num_fields + 1 > MAX_TAXA)
                {
                    fprintf(stderr, "Error: Number of taxa exceeds taxa max!\n");
                    return -1;
                }
                //shifts node_name_pointer to the next index of node_names AKA next field
                *active_node_map_pointer = num_fields;
                active_node_map_pointer++;
                num_fields = num_fields + 1;
                buffer_pointer = input_buffer;
                while (*buffer_pointer != '\0')
                {
                    *node_names_pointer = *buffer_pointer;
                    buffer_pointer++;
                    node_names_pointer++;
                } 
                *node_names_pointer = *buffer_pointer;
                buffer_pointer = input_buffer;
                node_names_pointer = *(node_names + num_fields);
                NODE *newNode = nodes_pointer;
                newNode -> name = *(node_names + (num_fields - 1));
                nodes_pointer++;
            }
            else if (current_character == '\n')
            {
                input_length = 0;
                //end of fields line
                //NULL terminating the current string field in node_names
                *buffer_pointer = '\0';
                if (num_fields + 1 > MAX_TAXA)
                {
                    fprintf(stderr, "Error: Number of taxa exceeds allowed taxa max!\n");
                    return -1;
                }
                //shifts node_name_pointer to the next index of node_names AKA next field
                *active_node_map_pointer = num_fields;
                num_fields = num_fields + 1;
                buffer_pointer = input_buffer;
                while (*buffer_pointer != '\0')
                {
                    *node_names_pointer = *buffer_pointer;
                    buffer_pointer++;
                    node_names_pointer++;
                } 
                *node_names_pointer = *buffer_pointer;
                buffer_pointer = input_buffer;
                //sets num_taxa to num_fields
                num_taxa = num_fields;
                //sets num_all_nodes to be initialized to num_taxa
                num_all_nodes = num_taxa;
                //sets num_active_nodes to be initialized to num_taxa
                num_active_nodes = num_taxa;
                node_names_pointer = *(node_names + num_fields);
                NODE *newNode = nodes_pointer;
                newNode -> name = *(node_names + (num_fields - 1));
                valid_line_count = valid_line_count + 1;
                character_count = 0;
            }
            else if (character_count != 1)
            {
                //checks for first blank taxa name and skips input
                *buffer_pointer = current_character;
                buffer_pointer++;
            } 
            character_count++;
        }
        else if (valid_line_count > 1 && valid_line_count < (num_taxa + 2))
        {
            if (character_count == 1)
            {
                while ((current_character != ',' && current_character != EOF))
                {
                    if (current_character != *(*(node_names + (valid_line_count - 2)) + (character_count - 1)))
                    {
                        fprintf(stderr, "Error: Incorrect taxa name in matrix!\n");
                        return -1;
                    }
                    current_character = fgetc(in);
                    character_count++;
                }
                if (*(*(node_names + (valid_line_count - 2)) + (character_count - 1)) != '\0')
                {
                    fprintf(stderr, "Error: Incorrect taxa name in matrix!\n");
                    return -1;
                }
            }
            if (character_count != 1)
            {
                buffer_pointer = (input_buffer + 0);
                current_character = fgetc(in);
                while ((current_character != EOF && current_character != '\n'))
                {
                    if (current_character != ',' && current_character != '\n')
                    {
                        if ((current_character < '0' || current_character > '9') && current_character != '.' )
                        {
                            fprintf(stderr, "Error: Matrix input is not a valid floating point value!\n");
                            return -1;
                        }
                        if (current_character == '.')
                        {
                            dot_count++;
                            if (dot_count > 1)
                            {
                                fprintf(stderr, "Error: Matrix input is not a valid floating point value!\n");
                                return -1;
                            }
                            char next_character = fgetc(in);
                            if (next_character > '9' || next_character < '0')
                            {
                                fprintf(stderr, "Error: Matrix input is not a valid floating point value!\n");
                                return -1;
                            }
                            ungetc(next_character, in);
                        }
                        if (current_character == '0')
                        {
                            char next_character = fgetc(in);
                            if (next_character != '.' && next_character != ',' && next_character != '\n' && next_character != EOF)
                            {
                                fprintf(stderr, "Error: Matrix input is not a valid floating point value!\n");
                                return -1;
                            }
                            ungetc(next_character, in);
                        }
                        *buffer_pointer = current_character;
                        buffer_pointer++;
                        current_character = fgetc(in);
                    }
                    if (current_character == ',')
                    {
                        current_character = fgetc(in);
                        if (current_character == ',' || current_character == '\n' || current_character == EOF)
                        {
                            fprintf(stderr, "Error: Missing input field!\n");
                            return -1;
                        }
                        ungetc(current_character, in);
                        *buffer_pointer = '\0';
                        buffer_pointer = (input_buffer + 0);
                        while (*buffer_pointer != '\0')
                        {
                            if (*buffer_pointer == '.')
                            {
                                buffer_pointer++;
                                float fraction = 0.1;
                                while (*buffer_pointer >= '0' && *buffer_pointer <= '9' && *buffer_pointer != '\0')
                                {
                                    value += (*buffer_pointer - '0') * fraction;
                                    fraction *= 0.1;
                                    buffer_pointer++;
                                }
                            }
                            else if (*buffer_pointer >= '0' && *buffer_pointer <= '9')
                            {
                                value = value * 10.0 + (*buffer_pointer - '0');
                                buffer_pointer++;
                            }
                            else
                            {
                                fprintf(stderr, "Error: Matrix input is not a valid floating point value!\n");
                                return -1;
                            }
                        }
                        distances_pointer = (*(distances + (valid_line_count - 2)) + num_row_nodes);
                        *distances_pointer = value;
                        value = 0.0;
                        dot_count = 0;
                        num_row_nodes++;
                        buffer_pointer = (input_buffer + 0);
                        current_character = fgetc(in);
                    }
                    else if (current_character == '\n')
                    {
                        *buffer_pointer = '\0';
                        buffer_pointer = (input_buffer + 0);
                        while (*buffer_pointer != '\0')
                        {
                            if (*buffer_pointer == '.')
                            {
                                buffer_pointer++;
                                float fraction = 0.1;
                                while (*buffer_pointer >= '0' && *buffer_pointer <= '9' && *buffer_pointer != '\0')
                                {
                                    value += (*buffer_pointer - '0') * fraction;
                                    fraction *= 0.1;
                                    buffer_pointer++;
                                }
                            }
                            else if (*buffer_pointer >= '0' && *buffer_pointer <= '9')
                            {
                                value = value * 10.0 + (*buffer_pointer - '0');
                                buffer_pointer++;
                            }
                            else
                            {
                                fprintf(stderr, "Error: Matrix input is not a valid floating point value!\n");
                                return -1;
                            }
                        }
                        if ((num_row_nodes + 1) > num_taxa || (num_row_nodes + 1) < num_taxa)
                        {
                            fprintf(stderr, "Error: Number of fields in matrix row is incorrect!\n");
                            return -1;
                        }
                        distances_pointer = (*(distances + (valid_line_count - 2)) + num_row_nodes);
                        *distances_pointer = value;
                        value = 0.0;
                        dot_count = 0;
                        character_count = 1;
                        num_row_nodes++;
                        valid_line_count = valid_line_count + 1;
                        num_row_nodes = 0;
                    }
                }
                if (current_character == EOF)
                {
                    *buffer_pointer = '\0';
                    buffer_pointer = (input_buffer + 0);
                    while (*buffer_pointer != '\0')
                    {
                        if (*buffer_pointer == '.')
                        {
                            buffer_pointer++;
                            float fraction = 0.1;
                            while (*buffer_pointer >= '0' && *buffer_pointer <= '9' && *buffer_pointer != '\0')
                            {
                                value += (*buffer_pointer - '0') * fraction;
                                fraction *= 0.1;
                                buffer_pointer++;
                            }
                        }
                        else if (*buffer_pointer >= '0' && *buffer_pointer <= '9')
                        {
                            value = value * 10.0 + (*buffer_pointer - '0');
                            buffer_pointer++;
                        }
                        else
                        {
                            fprintf(stderr, "Error: Matrix input is not a valid floating point value!\n");
                            return -1;
                        }
                    }
                    if ((num_row_nodes + 1) > num_taxa || (num_row_nodes + 1) < num_taxa)
                    {
                        fprintf(stderr, "Error: Number of fields in matrix row is incorrect!\n");
                        return -1;
                    }
                    distances_pointer = (*(distances + (valid_line_count - 2)) + num_row_nodes);
                    *distances_pointer = value;
                    value = 0.0;
                    dot_count = 0;
                    character_count = 1;
                    num_row_nodes++;
                    valid_line_count = valid_line_count + 1;
                    num_row_nodes = 0;
                }
            }
        }
    }
    for (int i = 0; i < num_taxa; i++)
    {
        for (int j = 0; j < num_taxa; j++)
        {
            if (i == j && (float)*(*(distances + i) + j) != (float)0.0)
            {
                fprintf(stderr, "Error: Non-zero along matrix diagonal!\n");
                return -1;
            }
            if (*(*(distances + i) + j) != *(*(distances + j) + i))
            {
                fprintf(stderr, "Error: Matrix is not symmetrical!\n");
                return -1;
            }
        }
    }
    return 0;
    abort();
}


/**
 * @brief  Emit a representation of the phylogenetic tree in Newick
 * format to a specified output stream.
 * @details  This function emits a representation in Newick format
 * of a synthesized phylogenetic tree to a specified output stream.
 * See (https://en.wikipedia.org/wiki/Newick_format) for a description
 * of Newick format.  The tree that is output will include for each
 * node the name of that node and the edge distance from that node
 * its parent.  Note that Newick format basically is only applicable
 * to rooted trees, whereas the trees constructed by the neighbor
 * joining method are unrooted.  In order to turn an unrooted tree
 * into a rooted one, a root will be identified according by the
 * following method: one of the original leaf nodes will be designated
 * as the "outlier" and the unique node adjacent to the outlier
 * will serve as the root of the tree.  Then for any other two nodes
 * adjacent in the tree, the node closer to the root will be regarded
 * as the "parent" and the node farther from the root as a "child".
 * The outlier node itself will not be included as part of the rooted
 * tree that is output.  The node to be used as the outlier will be
 * determined as follows:  If the global variable "outlier_name" is
 * non-NULL, then the leaf node having that name will be used as
 * the outlier.  If the value of "outlier_name" is NULL, then the
 * leaf node having the greatest total distance to the other leaves
 * will be used as the outlier.
 *
 * @param out  Stream to which to output a rooted tree represented in
 * Newick format.
 * @return 0 in case the output is successfully emitted, otherwise -1
 * if any error occurred.  If the global variable "outlier_name" is
 * non-NULL, then it is an error if no leaf node with that name exists
 * in the tree.
 */
int emit_newick_format(FILE *out) {
    // TO BE IMPLEMENTED
    return 0;
    abort();
}

/**
 * @brief  Emit the synthesized distance matrix as CSV.
 * @details  This function emits to a specified output stream a representation
 * of the synthesized distance matrix resulting from the neighbor joining
 * algorithm.  The output is in the same CSV form as the program input.
 * The number of rows and columns of the matrix is equal to the value
 * of num_all_nodes at the end of execution of the algorithm.
 * The submatrix that consists of the first num_leaves rows and columns
 * is identical to the matrix given as input.  The remaining rows and columns
 * contain estimated distances to internal nodes that were synthesized during
 * the execution of the algorithm.
 *
 * @param out  Stream to which to output a CSV representation of the
 * synthesized distance matrix.
 * @return 0 in case the output is successfully emitted, otherwise -1
 * if any error occurred.
 */
int emit_distance_matrix(FILE *out) {
    fprintf(stdout, ",");
    for (int i = 0; i < num_all_nodes; i++)
    {
        int node_name_counter = 0;
        while (*(*(node_names + i) + node_name_counter) != '\0')
        {
            fprintf(stdout, "%c", *(*(node_names + i) + node_name_counter));
            node_name_counter++;
        }
        if (i < num_all_nodes - 1)
        {
            fprintf(stdout, ",");
        }
    }
    fprintf(stdout, "\n");
    for (int i = 0; i < num_all_nodes; i++)
    {
        int node_name_counter = 0;
        while (*(*(node_names + i) + node_name_counter) != '\0')
        {
            fprintf(stdout, "%c", *(*(node_names + i) + node_name_counter));
            node_name_counter++;
        }
        fprintf(stdout, ",");
        for (int j = 0; j < num_all_nodes; j++)
        {
            fprintf(stdout,"%.2lf", *(*(distances + i) + j));
            if (j < num_all_nodes - 1)
            {
                fprintf(stdout, ",");
            }
        }
        fprintf(stdout, "\n");
    }
    return 0; 
    abort();
}

/**
 * @brief  Build a phylogenetic tree using the distance data read by
 * a prior successful invocation of read_distance_data().
 * @details  This function assumes that global variables and data
 * structures have been initialized by a prior successful call to
 * read_distance_data(), in accordance with the specification for
 * that function.  The "neighbor joining" method is used to reconstruct
 * phylogenetic tree from the distance data.  The resulting tree is
 * an unrooted binary tree having the N taxa from the original input
 * as its leaf nodes, and if (N > 2) having in addition N-2 synthesized
 * internal nodes, each of which is adjacent to exactly three other
 * nodes (leaf or internal) in the tree.  As each internal node is
 * synthesized, information about the edges connecting it to other
 * nodes is output.  Each line of output describes one edge and
 * consists of three comma-separated fields.  The first two fields
 * give the names of the nodes that are connected by the edge.
 * The third field gives the distance that has been estimated for
 * this edge by the neighbor-joining method.  After N-2 internal
 * nodes have been synthesized and 2*(N-2) corresponding edges have
 * been output, one final edge is output that connects the two
 * internal nodes that still have only two neighbors at the end of
 * the algorithm.  In the degenerate case of N=1 leaf, the tree
 * consists of a single leaf node and no edges are output.  In the
 * case of N=2 leaves, then no internal nodes are synthesized and
 * just one edge is output that connects the two leaves.
 *
 * Besides emitting edge data (unless it has been suppressed),
 * as the tree is built a representation of it is constructed using
 * the NODE structures in the nodes array.  By the time this function
 * returns, the "neighbors" array for each node will have been
 * initialized with pointers to the NODE structure(s) for each of
 * its adjacent nodes.  Entries with indices less than N correspond
 * to leaf nodes and for these only the neighbors[0] entry will be
 * non-NULL.  Entries with indices greater than or equal to N
 * correspond to internal nodes and each of these will have non-NULL
 * pointers in all three entries of its neighbors array.
 * In addition, the "name" field each NODE structure will contain a
 * pointer to the name of that node (which is stored in the corresponding
 * entry of the node_names array).
 *
 * @param out  If non-NULL, an output stream to which to emit the edge data.
 * If NULL, then no edge data is output.
 * @return 0 in case the output is successfully emitted, otherwise -1
 * if any error occurred.
 */
int build_taxonomy(FILE *out) {
    if (num_taxa == 2)
    {
        if (global_options != MATRIX_OPTION)
        {
            fprintf(stdout, "%d,%d,%.2lf\n", *(active_node_map + 0), *(active_node_map + (num_all_nodes - 1)), *(*(distances + 0) + (num_all_nodes - 1)));
        }
        return 0;
    }
    int edge_index = 0;
    double edge_data = 0;
    for (int n = 0; n <= num_taxa - 3; n++)
    { 
        //! Compute row sums for vector S(i)
        double *row_sums_pointer = (row_sums + 0);
        double current_sum = 0;
        for (int i = 0; i < num_active_nodes; i++)
        {
            row_sums_pointer = (row_sums + *(active_node_map + i));
            current_sum = 0;
            for (int j = 0; j < num_active_nodes; j++)
            {
                current_sum += *(*(distances + *(active_node_map + i)) + *(active_node_map + j));
            }
            *row_sums_pointer = current_sum;
        }
        //! Find the smallest distance pair
        //? Q(i,j) = (N-2) * D(i,j) - S(i) - S(j)
        double smallest_distance;
        double current_Q_value;
        int i_index;
        int j_index;
        for (int i = 0; i < num_active_nodes; i++)
        {
            for (int j = 0 + i; j < num_active_nodes; j++)
            {
                if (i != j)
                {
                    current_Q_value = (num_active_nodes - 2) * *(*(distances + *(active_node_map + i)) + *(active_node_map + j)) - *(row_sums + *(active_node_map + i)) - *(row_sums + *(active_node_map + j));
                    if (i == 0 && j == 1)
                    {
                        smallest_distance = current_Q_value;
                        i_index = *(active_node_map + i);
                        j_index = *(active_node_map + j);
                    }
                    if (current_Q_value < smallest_distance)
                    {
                        smallest_distance = current_Q_value;
                        i_index = *(active_node_map + i);
                        j_index = *(active_node_map + j);
                    }
                }
            }
        }
        
        //Create new node u, name #nnn
        NODE *newNode = (nodes + num_all_nodes);
        char *node_names_pointer = *(node_names + num_all_nodes);
        *node_names_pointer = '#';
        node_names_pointer++;
        sprintf(node_names_pointer, "%d", num_all_nodes);
        newNode -> name = *(node_names + num_all_nodes);
        int *active_node_map_pointer = (active_node_map + num_all_nodes);
        *active_node_map_pointer = num_all_nodes;

        //Join f with u and g with u
        double f_branch = ((*(*(distances + i_index) + j_index)/2) + (*(row_sums + i_index) - *(row_sums + j_index)) / (2 * (num_active_nodes - 2)));
        double g_branch = *(*(distances + i_index) + j_index) - f_branch;

        //& Print edge data
        if (global_options != MATRIX_OPTION)
        {
            fprintf(stdout, "%d,%d,%.2lf\n", *(active_node_map + i_index), *(active_node_map + num_all_nodes), f_branch);
            fprintf(stdout, "%d,%d,%.2lf\n", *(active_node_map + j_index), *(active_node_map + num_all_nodes), g_branch);
        }
        edge_data = g_branch;
        
        //sets u to parent (neighbors[0] of f and g)
        *((nodes + i_index)->neighbors + 0) = (nodes + num_all_nodes);
        *((nodes + j_index)->neighbors + 0) = (nodes + num_all_nodes);
        if (num_all_nodes <= num_taxa)
        {
            *((nodes + i_index)->neighbors + 1) = NULL;
            *((nodes + i_index)->neighbors + 2) = NULL;
            *((nodes + j_index)->neighbors + 1) = NULL;
            *((nodes + j_index)->neighbors + 2) = NULL;
        }
        *((nodes + num_all_nodes)->neighbors + 1) = ((nodes + i_index));
        *((nodes + num_all_nodes)->neighbors + 2) = ((nodes + j_index));
        //! Matrix Update
        /*
        ?    D'(u, k) = D'(k, u) =
        ?    if k = u then 0
        ?    else if k = f then D'(u, k) = (D(f, g) + (S(f) - S(g)) / (N - 2)) / 2
        ?    else if k = g then D'(u, k) = (D(f, g) + (S(g) - S(f)) / (N - 2)) / 2
        ?    else D'(u, k) = (D(f, k) + D(g, k) - D(f, g)) / 2
        */
        for (int k = 0; k <= num_all_nodes; k++)
        {
            if (num_active_nodes == k)
            {
                *(*(distances + *(active_node_map + num_all_nodes)) + (num_all_nodes)) = 0.0;
            }
            else if (k == *(active_node_map + i_index))
            {
                *(*(distances + *(active_node_map + num_all_nodes)) + *(active_node_map + k)) = f_branch;
                *(*(distances + *(active_node_map + k)) + *(active_node_map + num_all_nodes)) = f_branch;
            }
            else if (k == *(active_node_map + j_index))
            {
                *(*(distances + *(active_node_map + num_all_nodes)) + *(active_node_map + k)) = g_branch;
                *(*(distances + *(active_node_map + k)) + *(active_node_map + num_all_nodes)) = g_branch;
            }
            else
            {
                *(*(distances + num_all_nodes) + *(active_node_map + k)) = (*(*(distances + *(active_node_map + i_index)) + *(active_node_map + k)) + *(*(distances + *(active_node_map + j_index)) + *(active_node_map + k)) - (*(*(distances + *(active_node_map + i_index)) + *(active_node_map + j_index)))) / 2.0; 
                *(*(distances + *(active_node_map + k)) + num_all_nodes) = (*(*(distances + *(active_node_map + i_index)) + *(active_node_map + k)) + *(*(distances + *(active_node_map + j_index)) + *(active_node_map + k)) - (*(*(distances + *(active_node_map + i_index)) + *(active_node_map + j_index)))) / 2.0; 
            }
        }


        //deactivates f and g
        int node_map_index_i = 0;
        int node_map_index_j = 0;
        active_node_map_pointer = (active_node_map + 0);
        for (int i = 0; i < num_active_nodes; i++)
        {
            if (*active_node_map_pointer == i_index)
            {
                node_map_index_i = i;
            }
            if (*active_node_map_pointer == j_index)
            {
                node_map_index_j = i;
            }
            active_node_map_pointer++;
        }
        active_node_map_pointer = (active_node_map + node_map_index_i);
        *active_node_map_pointer = num_all_nodes;
        active_node_map_pointer = (active_node_map + node_map_index_j);
        edge_index = *(active_node_map + node_map_index_j);
        *active_node_map_pointer = *(active_node_map + (num_active_nodes - 1));
        num_all_nodes++;
        num_active_nodes--;
        if (num_active_nodes == 2)
        {
            //Setting neighbors for last remaining nodes correctly
            *((nodes + *(active_node_map + 0))->neighbors + 0) = *((nodes + *(active_node_map + 0))->neighbors + 1);
            *((nodes + *(active_node_map + 0))->neighbors + 1) = (nodes + *(active_node_map + 1));
            *((nodes + *(active_node_map + 1))->neighbors + 0) = (nodes + *(active_node_map + 0));
            *((nodes + *(active_node_map + 1))->neighbors + 1) = NULL;
            *((nodes + *(active_node_map + 1))->neighbors + 2) = NULL;
            //Join last remaining nodes 
            double last_branch = (*(*(distances + *(active_node_map + 1)) + edge_index)) - edge_data;
            //& Print edge data
            if (global_options != MATRIX_OPTION)
            {
                fprintf(stdout, "%d,%d,%.2lf\n", *(active_node_map + 1), *(active_node_map + 0), last_branch);
            }
            num_active_nodes = 0;
        }
    }
    if (num_all_nodes > MAX_NODES)
    {
        fprintf(stderr, "Error: Number of nodes exceeds maximum nodes!\n");
        return -1;
    }
    return 0;
    abort();
}
