#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#define file std::ifstream
#define arr std::vector<short>
#define matrix std::vector<std::vector<short>>
#define matrix1 std::vector<std::vector<std::pair<short, short>>>
#define u_map std::unordered_map<short, std::vector<int>>
#define lst std::list<short>
#define assign std::vector<short>

void read(file& f, matrix& clause_matrix, u_map& literals_to_clauses,
          lst& unar_clauses, assign& assigned_literals, int& clauses,
          int& literals) {
  int x, ind = 1;

  f >> literals >> clauses;
  assigned_literals.resize(literals + 1, 0);

  arr vec(literals + 1, 0);
  clause_matrix.push_back(vec);

  while (f >> x) {
    if (x != 0) {
      if (x > 0) {
        vec[x] = 1;
      } else {
        vec[-x] = -1;
      }

      ++vec[0];
      literals_to_clauses[x].push_back(ind);
    } else {
      if (vec[0] == 1) {
        for (int i = 1; i <= literals; ++i) {
          if (vec[i] != 0) {
            unar_clauses.push_back(i * vec[i]);
            break;
          }
        }
      }

      clause_matrix.push_back(vec);

      for (int i = 0; i <= literals; ++i) {
        vec[i] = 0;
      }
      ++ind;
    }
  }
}

matrix1 modify(int element, matrix& clause_matrix, u_map& literals_to_clauses,
               lst& unar_clauses, assign& assign_literals, int& clauses,
               int literals, int& k) {
  matrix1 matrix_modify(2);

  for (const int& item : literals_to_clauses[element]) {
    if (clause_matrix[item][0] != 0) {
      matrix_modify[0].push_back(std::make_pair(item, clause_matrix[item][0]));
      clause_matrix[item][0] = 0;
      --clauses;

      if (clauses == 0) {
        std::cout << "Formula este SATISFIABILA\n";

        for (int i = 1; i <= literals; ++ i) {
            std::cout << i << ": ";
            if (assign_literals[i] < 0) {
                std::cout << "negativ";
            } else if (assign_literals[i] > 0) {
                std::cout << "pozitiv";
            } else {
                std::cout << "orice valoare";
            }
            std::cout << "\n";
        }

        exit(0);
      }
    }
  }

  for (const int& item : literals_to_clauses[-element]) {
    if (clause_matrix[item][0] != 0) {
      matrix_modify[1].push_back(
          std::make_pair(item, clause_matrix[item][abs(element)]));
      clause_matrix[item][abs(element)] = 0;
      --clause_matrix[item][0];

      if (clause_matrix[item][0] == 0) {
        k = 1;
        break;
      } else if (clause_matrix[item][0] == 1) {
        for (int i = 1; i <= literals; ++i) {
          if (clause_matrix[item][i] != 0) {
            unar_clauses.push_back(i * clause_matrix[item][i]);
            break;
          }
        }
      }
    }
  }

  if (element < 0) {
    assign_literals[-element] = -1;
  } else {
    assign_literals[element] = 1;
  }

  return matrix_modify;
}

void undo_modify(int element, matrix1& matrix_modify, matrix& clause_matrix,
                 lst& unar_clauses, assign& assign_literals, int& clauses) {
  for (int i = 0; i < matrix_modify[0].size(); ++i) {
    clause_matrix[matrix_modify[0][i].first][0] = matrix_modify[0][i].second;
    ++clauses;
  }

  for (int i = 0; i < matrix_modify[1].size(); ++i) {
    if (clause_matrix[matrix_modify[1][i].first][0] == 1) {
      unar_clauses.pop_back();
    }
    ++clause_matrix[matrix_modify[1][i].first][0];
    clause_matrix[matrix_modify[1][i].first][abs(element)] =
        matrix_modify[1][i].second;
  }

  assign_literals[abs(element)] = 0;
}

int pick_next_literal(assign& assign_literals, int literals) {
  for (int i = 1; i <= literals; ++i) {
    if (assign_literals[i] == 0) {
      return i;
    }
  }
  return 0;
}

void DPLL(matrix& clause_matrix, u_map& literals_to_clauses, lst& unar_clauses,
          assign& assigned_literals, int& clauses, int literals) {
  if (unar_clauses.size() != 0) {
    int element = unar_clauses.back();
    unar_clauses.pop_back();

    int k = 0;
    matrix1 matrix_modify =
        modify(element, clause_matrix, literals_to_clauses, unar_clauses,
               assigned_literals, clauses, literals, k);

    if (k == 1) {
      undo_modify(element, matrix_modify, clause_matrix, unar_clauses,
                  assigned_literals, clauses);
    } else {
      DPLL(clause_matrix, literals_to_clauses, unar_clauses, assigned_literals,
           clauses, literals);
      undo_modify(element, matrix_modify, clause_matrix, unar_clauses,
                  assigned_literals, clauses);
    }

    unar_clauses.push_back(element);

  } else {
    int literal = pick_next_literal(assigned_literals, literals);
    int k = 0;

    for (int i = -1; i <= 1; i += 2) {
      matrix1 matrix_modify =
          modify(literal * i, clause_matrix, literals_to_clauses, unar_clauses,
                 assigned_literals, clauses, literals, k);

      if (k == 1) {
        undo_modify(literal * i, matrix_modify, clause_matrix, unar_clauses,
                    assigned_literals, clauses);
      } else {
        DPLL(clause_matrix, literals_to_clauses, unar_clauses,
             assigned_literals, clauses, literals);
        undo_modify(literal * i, matrix_modify, clause_matrix, unar_clauses,
                    assigned_literals, clauses);
      }
    }
  }
}

int main() {
  file f("input.txt");
  matrix clause_matrix;
  u_map literals_to_clauses;
  lst unar_clauses;
  assign assigned_literals;
  int clauses, literals;

  read(f, clause_matrix, literals_to_clauses, unar_clauses, assigned_literals,
       clauses, literals);

  DPLL(clause_matrix, literals_to_clauses, unar_clauses, assigned_literals,
       clauses, literals);

  std::cout << "Formula este NESATISFIABILA\n";

  return 0;
}