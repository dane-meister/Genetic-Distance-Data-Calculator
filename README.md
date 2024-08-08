# Genetic Distance Data Calculator

## Description 
This repository contains a C program designed to read, parse, and validate genetic distance data from an input file. It initializes the necessary data structures to store and process the distance values between different taxa.

## Deep Dive
The Genetic Distance Calculator program is a powerful tool designed to compute the genetic distances between different taxa, which is fundamental for constructing phylogenetic trees. Phylogenetic trees represent the evolutionary relationships among various species or taxa, with the distance between taxa indicating their genetic divergence. These distances are crucial for understanding evolutionary history, species classification, and biodiversity, making this program essential for evolutionary biology research and comparative genomics.
#### Importance of Genetic Distances
Genetic distances quantify the genetic divergence between different taxa, reflecting how closely or distantly related they are. These measurements are used to infer the evolutionary relationships among species, helping scientists construct phylogenetic trees. Phylogenetic trees provide a visual representation of these relationships, showing the branching patterns of evolution and allowing researchers to trace lineage divergence, identify common ancestors, and understand the genetic basis of speciation and adaptation.

#### Program Functionality and Implementation
The Genetic Distance Calculator program consists of three main components: main.c, philo.c, and validargs.c. Each component plays a crucial role in ensuring the accurate computation and validation of genetic distances.

- main.c:
The main.c file serves as the program's entry point, orchestrating the overall workflow. It begins by processing command-line arguments using the validargs.c module, which validates the input options and sets global flags accordingly. The program supports various options, such as specifying the input data format (matrix or Newick format) and defining outliers.

- validargs.c:
The validargs.c file handles the validation of command-line arguments. The int validargs(int argc, char **argv) function checks the input arguments for correctness, ensuring that only valid options are passed to the program. If the validation succeeds, the options are encoded in the global variable global_options; otherwise, the function returns -1, and an error message is displayed.

- philo.c:
The core functionality of the program resides in philo.c, which manages the reading, parsing, and validation of genetic distance data from an input file. The int read_distance_data(FILE *in) function reads the genetic distance data, typically formatted as a matrix. It processes each line of the input file, parsing the data into a symmetric matrix with zeroes on the main diagonal, indicating no distance between identical taxa.

During parsing, the function performs robust error handling to manage various error scenarios, including:

- Invalid Input Format: Ensuring the data forms a proper symmetric matrix.
- Inconsistent Taxa Names: Checking that taxa names are consistent and correctly formatted.
- Non-Numeric Values: Verifying that all distance values are numeric.
  
If any errors are encountered, the program prints an appropriate error message and terminates, ensuring data integrity.

Once the data is validated, the program initializes internal data structures to store the taxa names and their corresponding distances. These structures include:

- Taxa Names Array: An array to store the names of the taxa.
- Distances Matrix: A matrix to store the genetic distances between taxa.
- Node Representations: Structures to represent each node (taxon) in the analysis.
- Active Nodes Map: A map to keep track of active nodes during processing.
  
These data structures are crucial for efficiently managing and processing the genetic distances, enabling subsequent phylogenetic analysis.

#### Conclusion 
The Genetic Distance Calculator program ensures robust data validation and efficient data handling, providing a reliable tool for researchers to analyze genetic relationships and evolutionary patterns. By accurately calculating and validating genetic distances, the program facilitates the construction of reliable phylogenetic trees, aiding in the understanding of evolutionary biology and the genetic basis of biodiversity. This tool is indispensable for scientists seeking to explore the genetic connections between species and unravel the complexities of evolutionary history.

### Command-Line Options
- '-h': Displays help information.
- '-m': Specifies that the input data is in matrix format.
- '-n': Specifies that the input data is in Newick format.
- '-o <name>': Specifies an outlier name.

### Error Handling
The program includes robust error handling to manage various error scenarios, including:

- Invalid command-line arguments.
- Incorrect input data format.
- Exceeding maximum input field length.
- Inconsistent taxa names in the input matrix.
- Non-numeric values in the distance matrix.
  
#### License
This project is licensed under the MIT License. See the LICENSE file for details.
Disclaimer: This project was completed as a part of a university assignment.

