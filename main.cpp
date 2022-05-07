#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <string>
#include <locale>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <wchar.h>
#include <random>
#include <string_view>

#define edited 3_may
#define print_all_info
// #define need_to_write_additional_info

#define size_t unsigned long long

using namespace std;

bool Is_Russian_Letter(char c) {
    return (c >= "А"[0] and c <= "Я"[0]) or (c == "Ё"[0]);
}

double my_generate_random(double from, double to) {
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(from, to);
    double random_double = dis(gen);
    return random_double;
}

class Rat;

class Maze;

class Discrete_Operator {
public:
    double prob = 0;

    virtual void apply(Maze &maze, Rat &rat) {
        cerr << "Error: tried to apply general discrete operator\n";
        exit(1);
    }

    virtual void set_probability(double probability) {
        cerr << "Error: tried to set_probabilities for general discrete operator\n";
        exit(1);
    }

    virtual string get_name() {
        return "general";
    }
};

class Operator_Random : public Discrete_Operator {
public:
    void apply(Maze &maze, Rat &rat) override; // Написано ниже
    void set_probability(double probability) override; // Написано ниже
    string get_name() override {
        return "random";
    }
};

class Operator_Inversion : public Discrete_Operator {
public:
    int max_len_of_path_a_rat_can_invert = 5;

    void apply(Maze &maze, Rat &rate) override; // Написано ниже

    void set_probability(double probability) override; // Написано ниже

    string get_name() override {
        return "inversion";
    }
};

class Operator_Symmetry : public Discrete_Operator {
public:
    void apply(Maze &maze, Rat &rat) override; // Написано ниже

    void set_probability(double probability) override; // Написано ниже

    string get_name() override {
        return "symmetry";
    }
};

class Operator_Circle : public Discrete_Operator {
public:
    void apply(Maze &maze, Rat &rat) override; // Написано ниже

    void set_probability(double probability) override; // Написано ниже

    string get_name() override {
        return "circle";
    }
};

double approximately_one = 0.9; // Для выведения медиан
double eps = 0.001; // Для устранения неточности вычисления в double

double INF = exp(100);

enum class abs_direction { // absolute direction
    left, right, up, down, not_chosen
};

enum class relative_direction {
    to_left, to_right, forward, backwards
};


enum class operator_types {
    Discrete_Operator, Operator_Random, Operator_Inversion, Operator_Symmetry, Operator_Circle
};


enum class type_of_maze {
    nikolskaya, chelnok, second_chelnok, not_set
};

class Cell {
public:
    int num = 0;
    unordered_map<abs_direction, int> neighs;
};

int n;
unordered_map<char, int> char_to_int;
string int_to_char;

abs_direction determine_new_direction(abs_direction cur_direction, relative_direction choice) {
    if (choice == relative_direction::forward) {
        if (cur_direction == abs_direction::left) {
            return abs_direction::left;
        }
        if (cur_direction == abs_direction::right) {
            return abs_direction::right;
        }
        if (cur_direction == abs_direction::up) {
            return abs_direction::up;
        }
        if (cur_direction == abs_direction::down) {
            return abs_direction::down;
        }
    }
    if (choice == relative_direction::backwards) {
        if (cur_direction == abs_direction::left) {
            return abs_direction::right;
        }
        if (cur_direction == abs_direction::right) {
            return abs_direction::left;
        }
        if (cur_direction == abs_direction::up) {
            return abs_direction::down;
        }
        if (cur_direction == abs_direction::down) {
            return abs_direction::up;
        }
    }
    if (choice == relative_direction::to_left) {
        if (cur_direction == abs_direction::left) {
            return abs_direction::down;
        }
        if (cur_direction == abs_direction::up) {
            return abs_direction::left;
        }
        if (cur_direction == abs_direction::right) {
            return abs_direction::up;
        }
        if (cur_direction == abs_direction::down) {
            return abs_direction::right;
        }
    }
    if (choice == relative_direction::to_right) {
        if (cur_direction == abs_direction::left) {
            return abs_direction::up;
        }
        if (cur_direction == abs_direction::up) {
            return abs_direction::right;
        }
        if (cur_direction == abs_direction::right) {
            return abs_direction::down;
        }
        if (cur_direction == abs_direction::down) {
            return abs_direction::left;
        }
    }
    cerr << "Error #2 from determine_new_direction!!!\n";
    exit(1);
}

class Rat {
public:
    int cur_cell_int = 0;
    abs_direction cur_direction;
    abs_direction which_feeder_we_start_from = abs_direction::not_chosen;

    unordered_map<relative_direction, double> probabilities_of_relative_dirs;
    unordered_map<abs_direction, double> cur_probabilities_of_abs_dirs;
    type_of_maze typeOfMaze = type_of_maze::not_set;

    void set_probabilities_of_relative_dirs(double pr_forward, double pr_right, double pr_left, double pr_back) {
        if (abs(pr_forward + pr_right + pr_left + pr_back - 1) > eps) {
            cerr << "Error! Incorrect probabilities for a rat!\n";
            exit(1);
        }
        probabilities_of_relative_dirs[relative_direction::to_right] = pr_right;
        probabilities_of_relative_dirs[relative_direction::to_left] = pr_left;
        probabilities_of_relative_dirs[relative_direction::forward] = pr_forward;
        probabilities_of_relative_dirs[relative_direction::backwards] = pr_back;
    }

    bool completed_route() {
        if (typeOfMaze == type_of_maze::nikolskaya) {
            return (cur_cell_int == char_to_int["Р"[0]]) || (cur_cell_int == char_to_int["Т"[0]]);
        } else if (typeOfMaze == type_of_maze::chelnok) {
            return ((which_feeder_we_start_from == abs_direction::right &&
                     cur_cell_int == char_to_int["П"[0]])
                    || (which_feeder_we_start_from == abs_direction::left &&
                        cur_cell_int == char_to_int["Н"[0]]));
        } else if (typeOfMaze == type_of_maze::second_chelnok) {
            return ((which_feeder_we_start_from == abs_direction::right &&
                     cur_cell_int == char_to_int["Р"[0]])
                    || (which_feeder_we_start_from == abs_direction::left &&
                        cur_cell_int == char_to_int["Н"[0]]));
        } else if (typeOfMaze == type_of_maze::not_set) {
            cerr << "Error in bool completed_route() : typeOfMaze == not_set\n";
        } else {
            cerr << "Error in bool completed_route() because of strange typeOfMaze\n";
        }
        exit(1);
    }

    abs_direction make_decision() {
        double check_sum = 0;
        for (auto &pair_direction_double: cur_probabilities_of_abs_dirs) {
            double cur_prob = pair_direction_double.second;
            if (cur_prob < 0.0) {
                cerr << "Error! Negative probability in make_decision!\n";
                exit(1);
            }
            check_sum += cur_prob;
        }
        if (abs(check_sum - 1) > eps) {
            cerr << "Error! Wrong sum of current probabilities in make_decision!\n";
            exit(1);
        }

        // Link: https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
        double random_from_0_to_1 = my_generate_random(0.0, 1.0);
        double cur_sum = 0.0;
        for (auto &pair_direction_double: cur_probabilities_of_abs_dirs) {
            abs_direction dir = pair_direction_double.first;
            double prob = pair_direction_double.second;
            if (prob != 0.0) {
                cur_sum += prob;
                if (cur_sum + eps > random_from_0_to_1) {
                    return dir;
                }
            }
        }
        cerr << "Error in make_decision. NO direction has been chosen\n";
        exit(1);
    }
};

class Maze {
public:
    type_of_maze typeOfMaze = type_of_maze::not_set;

    vector<Cell> cells;
    string cur_generated_route;
    vector<string> vector_of_generated_routes;

    vector<string> vector_of_real_routes;
    map<string, vector<vector<string>>> rats_name_to_experiment_results;

    Discrete_Operator discrete_operator;
    Operator_Random operator_random;
    Operator_Inversion operator_inversion;
    Operator_Symmetry operator_symmetry;
    Operator_Circle operator_circle;

    unordered_map<string, Discrete_Operator *> map_from_name_to_operator;
    unordered_map<string, double> operators_to_probabilities;

    double average_len_of_real_routes = -1;
    long long median_len_of_real_routes = -1;
    vector<long long> most_frequent_lens_of_real_routes = {-1};
    double average_num_of_mistakes_for_real_routes = -1;
    long long median_num_of_mistakes_for_real_routes = -1;
    vector<long long> most_frequent_num_of_mistakes_for_real_routes = {-1};

    double average_len_of_generated_routes = -1;
    long long median_len_of_generated_routes = -1;
    vector<long long> most_frequent_lens_of_generated_routes = {-1};
    double average_num_of_mistakes_for_generated_routes = -1;
    long long median_num_of_mistakes_for_generated_routes = -1;
    vector<long long> most_frequent_num_of_mistakes_for_generated_routes = {-1};

    double cur_value_of_optimization_function = INF;
    double optimum = INF;
    vector<operator_types> what_operators_are_optimized;

    bool flag_has_to_stop = false;

    bool route_from_input_is_correct(std::string_view s) {
        if (typeOfMaze == type_of_maze::nikolskaya) {
            if (!((s.back() == "Р"[0]) || (s.back() == "Т"[0])) && (s.front() == "О"[0])) {
                return false;
            }
            size_t counter_of_terminal_letters = 0;
            for (size_t i = 0; i < s.length(); ++i) {
                counter_of_terminal_letters += ((s[i] == "Р"[0]) || (s[i] == "Т"[0]));
            }
            return counter_of_terminal_letters == 1;
        } else if (typeOfMaze == type_of_maze::chelnok) {
            if (s.length() < 3) {
                return false;
            }
            int index_for_last_symbol = (int) (s.length()) - 1;
            if (s[index_for_last_symbol] == '+') {
                --index_for_last_symbol;
            }
            char last_symb = s[index_for_last_symbol];
            bool back_is_OK = (last_symb == "П"[0] or last_symb == "Ш"[0] or last_symb == "Ё"[0] or last_symb == "С"[0]
                               or last_symb == "Н"[0] or last_symb == "Щ"[0] or last_symb == "Й"[0] or
                               last_symb == "У"[0]);
            bool beginning_is_OK = (s[0] == "П"[0] or s[0] == "Ш"[0] or s[0] == "Ё"[0] or s[0] == "С"[0]
                                    or s[0] == "Н"[0] or s[0] == "Щ"[0] or s[0] == "Й"[0] or s[0] == "У"[0]);;

            return back_is_OK && beginning_is_OK;
        } else if (typeOfMaze == type_of_maze::second_chelnok) {
            if (s.length() < 3) {
                return false;
            }
            int index_for_last_symbol = (int) (s.length()) - 1;
            if (s[index_for_last_symbol] == '+') {
                --index_for_last_symbol;
            }
            char last_symb = s[index_for_last_symbol];
            bool back_is_OK = (last_symb == "Р"[0] or last_symb == "Ш"[0] or last_symb == "Т"[0] or last_symb == "С"[0]
                               or last_symb == "Н"[0] or last_symb == "Щ"[0] or last_symb == "Й"[0] or
                               last_symb == "У"[0]);
            bool beginning_is_OK = (s[0] == "Р"[0] or s[0] == "Ш"[0] or s[0] == "Т"[0] or s[0] == "С"[0]
                                    or s[0] == "Н"[0] or s[0] == "Щ"[0] or s[0] == "Й"[0] or s[0] == "У"[0]);;
            return back_is_OK && beginning_is_OK;
        } else if (typeOfMaze == type_of_maze::not_set) {
            cerr << "Error in bool route_from_input_is_correct(std::string_view s) : typeOfMaze == not_set\n";
        } else {
            cerr << "Error in bool route_from_input_is_correct(std::string_view s) because of strange typeOfMaze\n";
        }
        exit(1);
    }

    size_t get_number_of_mistakes_in_route(string_view route) {
        unordered_map<char, size_t> from_char_to_counter_of_visits;
        size_t ans = 0;
        for (char cell_char: route) {
            ++from_char_to_counter_of_visits[cell_char];
            if (from_char_to_counter_of_visits[cell_char] > 1) {
                ++ans;
            }
        }
        return ans;
    }

    Maze() {
        int_to_char = "_АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯRSWZDFQVUIJGLfd";
        n = int_to_char.size();
        for (size_t i = 0; i < int_to_char.length(); ++i) {
            char_to_int[int_to_char[i]] = i;
        }
        cells.resize(n);
    }

    void construct(istream &input, ostream &output) {
        string line;
        while (getline(input, line)) {
            stringstream ss(line);
            char cur_cell_char, left_neigh_char, up_neigh_char, right_neigh_char, down_neigh_char;
            ss >> cur_cell_char >> left_neigh_char >> up_neigh_char >> right_neigh_char >> down_neigh_char;
            int cur_cell_int = char_to_int[cur_cell_char];
            int left_neigh_int = char_to_int[left_neigh_char];
            int up_neigh_int = char_to_int[up_neigh_char];
            int right_neigh_int = char_to_int[right_neigh_char];
            int down_neigh_int = char_to_int[down_neigh_char];
            add_neighs_to_cell(cur_cell_int, left_neigh_int, up_neigh_int, right_neigh_int, down_neigh_int);
        }
    }

    void add_neighs_to_cell(int cur_int, int left_int, int up_int, int right_int, int down_int) {
        cells[cur_int].num = cur_int;
        cells[cur_int].neighs[abs_direction::left] = left_int;
        cells[cur_int].neighs[abs_direction::up] = up_int;
        cells[cur_int].neighs[abs_direction::right] = right_int;
        cells[cur_int].neighs[abs_direction::down] = down_int;
    }

    void print_info_about_cells_in_numbers(istream &input, ostream &output) {
        for (size_t i = 0; i < n; ++i) {
            output << "Клетка " << i << ' ';
            if (cells[i].num == 0) {
                output << "не существует";
            } else {
                unordered_map<abs_direction, int> &neighs = cells[i].neighs;
                output << int_to_char[i] << " имеет соседей " << neighs[abs_direction::left] << ' '
                       << neighs[abs_direction::up]
                       << ' ' << neighs[abs_direction::right] << ' ' << neighs[abs_direction::down];
            }
            output << '\n';
        }
    }

    void print_info_about_cells_in_letters(ostream &output) {
        for (size_t i = 0; i < n; ++i) {
            output << "Клетка " << int_to_char[i] << ' ';
            if (cells[i].num == 0) {
                output << "не существует";
            } else {
                unordered_map<abs_direction, int> &neighs = cells[i].neighs;
                output << i << " имеет соседей " << int_to_char[neighs[abs_direction::left]] << ' '
                       << int_to_char[neighs[abs_direction::up]]
                       << ' ' << int_to_char[neighs[abs_direction::right]] << ' '
                       << int_to_char[neighs[abs_direction::down]];
            }
            output << '\n';
        }
    }

    string choose_an_operator() {

        // Проверка того, что данные корректные
        double sum_of_probs = 0;
        for (auto &pair_string_double: operators_to_probabilities) {
            double el = pair_string_double.second;
            if (el < 0) {
                cerr << "Error in choose_an_operator. There is a negative probability!\n";
                exit(1);
            }
            sum_of_probs += el;
        }
        if (abs(1 - sum_of_probs) > eps) {
            cerr << "Error in choose_an_operator. Sum of probabilities is not equal to 1.0!\n";
            exit(1);
        }

        double random_from_0_to_1 = my_generate_random(0, 1.0);
        double cur_sum = 0.0;
        for (auto &pair_string_double: operators_to_probabilities) {
            string name_of_operator = pair_string_double.first;
            double prob = pair_string_double.second;
            if (prob != 0.0) {
                cur_sum += prob;
                if (cur_sum + eps > random_from_0_to_1) {
                    return name_of_operator;
                }
            }
        }
        cerr << "Error in choose_an_operator. NO operator had been chosen\n";
        exit(1);
    }

    void conduct_experiment(Rat &roger) {
#ifdef need_to_write_additional_info
        cout << "\nNew experiment began\n";
#endif
        cur_generated_route = "";
        char start_char = '0';
        if (typeOfMaze == type_of_maze::nikolskaya) {
            start_char = "Ы"[0];
            roger.cur_direction = abs_direction::down;
        } else if (typeOfMaze == type_of_maze::chelnok) {
            double random_value = my_generate_random(0.0, 2.0);
            if (random_value > 1) {
                start_char = "Н"[0];
                roger.which_feeder_we_start_from = abs_direction::right;
                roger.cur_direction = abs_direction::left;
            } else {
                start_char = "П"[0];
                roger.which_feeder_we_start_from = abs_direction::left;
                roger.cur_direction = abs_direction::right;
            }
        } else if (typeOfMaze == type_of_maze::second_chelnok) {
            double random_value = my_generate_random(0.0, 2.0);
            if (random_value > 1) {
                start_char = "Н"[0];
                roger.which_feeder_we_start_from = abs_direction::right;
                roger.cur_direction = abs_direction::left;
            } else {
                start_char = "Р"[0];
                roger.which_feeder_we_start_from = abs_direction::left;
                roger.cur_direction = abs_direction::right;
            }
        } else {
            cerr << "Error from conduct_experiment: strange maze type\n";
            exit(1);
        }
        if (start_char == '0') {
            cerr << "Error in void conduct_experiment(Rat &roger) : strange start_char\n";
            exit(1);
        }
        roger.cur_cell_int = char_to_int[start_char];
        int i = 0;
        while (!roger.completed_route() && i <= 1000) {
            string chosen_operator = choose_an_operator();
            // Здесь можно выводить информацию от том, какой дискретный оператор в данный момент работает.
            // chosen_operator - это имя дискретного оператора, применяемого в данный момент.
            if (chosen_operator != "random" && chosen_operator != "inversion" && chosen_operator != "symmetry" &&
                chosen_operator != "circle") {
                cerr << "Error: operator other than random or inversion has been chosen!..\n";
                exit(1);
            }
            map_from_name_to_operator[chosen_operator]->apply(*this, roger);
#ifdef need_to_write_additional_info
            if (chosen_operator == "random") {
                cout << chosen_operator;
                cout << " ";
                if (roger.cur_direction == abs_direction::right) {
                    cout << "right";
                } else if (roger.cur_direction == abs_direction::up) {
                    cout << "up";
                } else if (roger.cur_direction == abs_direction::left) {
                    cout << "left";
                } else if (roger.cur_direction == abs_direction::down) {
                    cout << "down";
                }
            } else if (chosen_operator == "inversion") {
                cout << chosen_operator << " applied";
            } else if (chosen_operator == "symmetry") {
                cout << chosen_operator << " applied";
            }
            else if (chosen_operator == "circle") {
                cout << chosen_operator << "appliend";
            }
            else {
                cerr << "Error: strange operator!\n";
                exit(1);
            }
            cout << ". Now we are in " << char_to_int[cur_generated_route.back()] << ' ' << cur_generated_route.back();
            cout << '\n';
#endif
            ++i;
        }
        if (i > 1000) {
            flag_has_to_stop = true;
        }
    }

    void conduct_series_of_experiments_memorizing_routes(size_t number_of_experiments, Rat &roger) {
        vector_of_generated_routes.clear();
        flag_has_to_stop = false;
        roger.typeOfMaze = typeOfMaze;
        long long counter_of_wrong_routes = 0;
        for (size_t i = 0; i < number_of_experiments; ++i) {
            conduct_experiment(roger);
            if (flag_has_to_stop) {
                return;
            }
            string route_to_be_added = get_simplified_route(cur_generated_route);
            if (route_to_be_added.length() < 100) {
                vector_of_generated_routes.push_back(route_to_be_added);
            } else {
                if (++counter_of_wrong_routes > 0.3 * number_of_experiments) {
                    // Если хотя бы 30% экспериментов в серии получаются странными,
                    // то есть приводят к слишком длинным путям, мы завершаем серию экспериментов
                    flag_has_to_stop = true;
                    return;
                }
            }
        }
    }

    string get_simplified_route(const string &route_to_simplify) {
        string simplified_route;
        for (size_t i = 0; i < route_to_simplify.length(); ++i) {
            char c = route_to_simplify[i];
            if (Is_Russian_Letter(c)) {
                if (simplified_route.empty() || simplified_route.back() != c) {
                    simplified_route.push_back(c);
                }
            }
        }
        return simplified_route;
    }

    abs_direction get_abs_direction_from_cell_number_to_cell_number(int from_num, int to_num) {
        vector<abs_direction> all_abs_directions{abs_direction::left,
                                                 abs_direction::up,
                                                 abs_direction::right,
                                                 abs_direction::down};

        for (auto cur_abs_dir: all_abs_directions) {
            if (cells[from_num].neighs[cur_abs_dir] == to_num) {
                return cur_abs_dir;
            }
        }
        cerr << "Error in get_abs_direction_from_cell_number_to_cell_number! Cannot determine direction\n";
        cerr << "from_num = " << from_num << "\nto_num = " << to_num << '\n';
        exit(1);
    }

    void construct_map_from_name_to_operator() {
        map_from_name_to_operator[discrete_operator.get_name()] = &discrete_operator;
        map_from_name_to_operator[operator_random.get_name()] = &operator_random;
        map_from_name_to_operator[operator_inversion.get_name()] = &operator_inversion;
        map_from_name_to_operator[operator_symmetry.get_name()] = &operator_symmetry;
        map_from_name_to_operator[operator_circle.get_name()] = &operator_circle;
    }

    void set_probabilities_for_operators(vector<double> vec) {
        if (vec.size() != 5) {
            cerr << "Error in set_probabilities_for_operators. Wrong size of vector!\n";
            exit(1);
        }
        double sum_of_probs = 0;
        for (double el: vec) {
            if (el < 0) {
                cerr << "Error in set_probabilities_for_operators. Found a negative probability!\n";
                exit(1);
            }
            sum_of_probs += el;
        }
        if (abs(1 - sum_of_probs) > eps) {
            cerr << "Error in set_probabilities_for_operators. Sum of probabilities is not equal to 1.0!\n";
            exit(1);
        }
        operators_to_probabilities[discrete_operator.get_name()] = vec[0];
        operators_to_probabilities[operator_random.get_name()] = vec[1];
        operators_to_probabilities[operator_inversion.get_name()] = vec[2];
        operators_to_probabilities[operator_symmetry.get_name()] = vec[3];
        operators_to_probabilities[operator_circle.get_name()] = vec[4];
    }

    void print_all_generated_routes(ostream &o) {
        for (size_t i = 0; i < vector_of_generated_routes.size(); ++i) {
            o << "\nRoute " << i + 1 << ":\n";
            o << vector_of_generated_routes[i] << "\n";
        }
    }

    char process_random_move_of_rat(Rat *rat_ptr) {
        // функция, отвечающая за перемещение крысы из текущего участка лабиринта в соседний.
        // Возвращает char -- букву (русскую или английскую), характеризующую позицию крысы в лабиринте
        int cur_cell_int = rat_ptr->cur_cell_int;
        abs_direction cur_direction = rat_ptr->cur_direction;
        // int trying_to_move_to_cell = cells[cur_cell_int].neighs[cur_direction];
        vector<relative_direction> all_relative_directions = {relative_direction::forward,
                                                              relative_direction::to_left,
                                                              relative_direction::to_right,
                                                              relative_direction::backwards};
        double sum_of_probabilities_impossible = 0;
        for (relative_direction choice: all_relative_directions) {
            abs_direction new_direction = determine_new_direction(cur_direction, choice);
            if (0 == cells[cur_cell_int].neighs[new_direction]) {
                sum_of_probabilities_impossible += rat_ptr->probabilities_of_relative_dirs[choice];
                rat_ptr->cur_probabilities_of_abs_dirs[new_direction] = 0;
            }
        } // вычислили суммарную вероятность направлений, в которых двигаться невозможно

        if (abs(1 - sum_of_probabilities_impossible) < eps) {
            cerr << "vertex number " << cur_cell_int << " " << int_to_char[cur_cell_int] << '\n';
            cerr
                    << "Error in function process_move_of_rat! We have no possible directions! (In processing rat move we almost divided by zero!)\n";
            cerr << "cur_cell = " << rat_ptr->cur_cell_int << "\n";
            cerr << cur_generated_route << '\n';
            exit(1);
        }
        for (relative_direction choice: all_relative_directions) {
            abs_direction new_direction = determine_new_direction(cur_direction, choice);
            if (0 != cells[cur_cell_int].neighs[new_direction]) {
                rat_ptr->cur_probabilities_of_abs_dirs[new_direction] =
                        rat_ptr->probabilities_of_relative_dirs[choice] *
                        (1 + sum_of_probabilities_impossible / (1 - sum_of_probabilities_impossible));
            }
        }
        abs_direction dir = rat_ptr->make_decision();
        int new_cell_int = cells[cur_cell_int].neighs[dir];
        rat_ptr->cur_cell_int = new_cell_int;
        rat_ptr->cur_direction = dir;
        return int_to_char[new_cell_int];
    }

    double get_average_length_of_generated_routes() {
        if (vector_of_generated_routes.empty()) {
            cerr << "Error in double get_average_length_of_generated_route() : empty vector_of_generated_routes\n";
            exit(1);
        }
        uint64_t sum = 0;
        for (auto &route: vector_of_generated_routes) {
            sum += route.length();
        }
        return double(sum) / vector_of_generated_routes.size();
    }

    vector<size_t> get_most_frequent_lengths_of_generated_routes() {
        if (vector_of_generated_routes.empty()) {
            cerr << "Error in vector<size_t> get_most_frequent_lengths_of_generated_route() : empty vector_of_generated_routes\n";
            exit(1);
        }
        unordered_map<size_t, size_t> lengths_to_number_of_occurrences;
        int64_t max_num_of_occurrences = -1;
        for (auto &route: vector_of_generated_routes) {
            int64_t cur = ++lengths_to_number_of_occurrences[route.length()];
            if (cur > max_num_of_occurrences) {
                max_num_of_occurrences = cur;
            }
        }
        vector<size_t> most_frequent_lengths_of_route;
        for (auto &pair_length_number: lengths_to_number_of_occurrences) {
            if (pair_length_number.second >= approximately_one * max_num_of_occurrences) {
                most_frequent_lengths_of_route.push_back(pair_length_number.first);
            }
        }
        return most_frequent_lengths_of_route;
    }

    size_t get_median_length_of_generated_routes() {
        if (vector_of_generated_routes.empty()) {
            cerr << "Error in size_t get_median_length_of_generated_routes() : empty vector_of_generated_routes\n";
            exit(1);
        }
        vector<size_t> lengths_of_routes;
        for (auto &route: vector_of_generated_routes) {
            lengths_of_routes.push_back(route.length());
        }
        sort(lengths_of_routes.begin(), lengths_of_routes.end());
        return lengths_of_routes[lengths_of_routes.size() / 2];
    }

    double get_average_num_of_mistakes_for_generated_routes() {
        if (vector_of_generated_routes.empty()) {
            cerr << "Error in double get_average_num_of_mistakes_for_generated_routes() : empty vector_of_generated_routes\n";
            exit(1);
        }
        size_t num_of_mistakes_in_all_routes = 0;
        for (string_view s: vector_of_generated_routes) {
            num_of_mistakes_in_all_routes += get_number_of_mistakes_in_route(s);
        }
        return double(num_of_mistakes_in_all_routes) / vector_of_generated_routes.size();
    }

    size_t get_median_num_of_mistakes_for_generated_routes() {
        if (vector_of_generated_routes.empty()) {
            cerr << "Error in size_t get_median_num_of_mistakes_for_generated_routes() : empty vector_of_generated_routes\n";
            exit(1);
        }
        vector<size_t> nums_of_mistakes;
        for (string_view route: vector_of_generated_routes) {
            nums_of_mistakes.push_back(get_number_of_mistakes_in_route(route));
        }
        sort(nums_of_mistakes.begin(), nums_of_mistakes.end());
        return nums_of_mistakes[nums_of_mistakes.size() / 2];
    }

    vector<size_t> get_most_frequent_num_of_mistakes_for_generated_routes() {
        if (vector_of_generated_routes.empty()) {
            cerr << "Error in vector<size_t> get_most_frequent_num_of_mistakes_for_generated_routes() : empty vector_of_generated_routes\n";
            exit(1);
        }
        unordered_map<size_t, size_t> num_of_mistakes_to_num_of_occurrences;
        int64_t max_num_of_occurrences = -1;
        for (string_view route: vector_of_generated_routes) {
            int64_t cur = ++num_of_mistakes_to_num_of_occurrences[get_number_of_mistakes_in_route(route)];
            if (cur > max_num_of_occurrences) {
                max_num_of_occurrences = cur;
            }
        }
        vector<size_t> most_frequent_num_of_mistakes;
        for (auto &pair: num_of_mistakes_to_num_of_occurrences) {
            if (pair.second >= approximately_one * max_num_of_occurrences) {
                most_frequent_num_of_mistakes.push_back(pair.first);
            }
        }
        return most_frequent_num_of_mistakes;
    }

    void print_statistical_data(ostream &o) {
        if (!vector_of_generated_routes.empty()) {
            o << "Для смоделированной крысы:\nСреднее арифметическое длин пути = "
              << get_average_length_of_generated_routes() << '\n';
            o << "Медианная длина пути = " << get_median_length_of_generated_routes() << '\n';
            o << "Самые частые длины пути : ";
            for (size_t ans: get_most_frequent_lengths_of_generated_routes()) {
                o << ans << "\t";
            }
            o << "\nСреднее число ошибок: " << get_average_num_of_mistakes_for_generated_routes();
            o << "\nМедиана числа ошибок: " << get_median_num_of_mistakes_for_generated_routes() << '\n';
            o << "Наиболее часто встречающиеся числа ошибок: ";
            for (auto el: get_most_frequent_num_of_mistakes_for_generated_routes()) {
                o << el << " ";
            }
            o << '\n' << "--------------------------------------" << '\n';
        } else {
            o << "Для смоделированной крысы ничего не печатаем,\n";
            o << " так как vector_of_generated_routes.size() == 0\n-------------------------------------------------\n";
        }

        if (!vector_of_real_routes.empty()) {
            o << "Для реальной крысы:\nСреднее арифметическое длин пути = " << get_average_length_of_real_route()
              << '\n';
            o << "Медианная длина пути = " << get_median_length_of_real_route() << '\n';
            o << "Самые частые длины пути : ";
            for (size_t ans: get_most_frequent_lengths_of_real_route()) {
                o << ans << "\t";
            }
            o << '\n';
            o << "Среднее число ошибок: " << get_average_num_of_mistakes_for_real_routes();
            o << "\nМедиана числа ошибок: " << get_median_num_of_mistakes_for_real_routes() << '\n';
            o << "Наиболее часто встречающиеся числа ошибок: ";
            for (auto el: get_most_frequent_num_of_mistakes_for_real_routes()) {
                o << el << " ";
            }
            o << '\n';
        } else {
            o << "Для реальной крысы ничего не печатаем,";
            o << " так как vector_of_real_routes.size() == 0\n-------------------------------------------------\n";
        }
    }

    double get_average_length_of_real_route() {
        if (vector_of_real_routes.empty()) {
            cerr << "Error in double get_average_length_of_real_route() : empty vector_of_real_routes\n";
            exit(1);
        }
        uint64_t sum = 0;
        for (auto &route: vector_of_real_routes) {
            sum += route.length();
        }
        return double(sum) / vector_of_real_routes.size();
    }

    size_t get_median_length_of_real_route() {
        if (vector_of_real_routes.empty()) {
            cerr << "Error in size_t get_median_length_of_real_route() : empty vector_of_real_routes\n";
            exit(1);
        }
        vector<size_t> lengths_of_routes;
        for (auto &route: vector_of_real_routes) {
            lengths_of_routes.push_back(route.length());
        }
        sort(lengths_of_routes.begin(), lengths_of_routes.end());
        return lengths_of_routes[lengths_of_routes.size() / 2];
    }

    vector<size_t> get_most_frequent_lengths_of_real_route() {
        if (vector_of_real_routes.empty()) {
            cerr << "Error in vector<size_t> get_most_frequent_lengths_of_real_route() : empty vector_of_real_routes\n";
            exit(1);
        }
        unordered_map<size_t, size_t> lengths_to_number_of_occurrences;
        int64_t max_num_of_occurrences = -1;
        for (auto &route: vector_of_real_routes) {
            int64_t cur = ++lengths_to_number_of_occurrences[route.length()];
            if (cur > max_num_of_occurrences) {
                max_num_of_occurrences = cur;
            }
        }
        vector<size_t> most_frequent_lengths_of_route;
        for (auto &pair_length_number: lengths_to_number_of_occurrences) {
            if (pair_length_number.second >= approximately_one * max_num_of_occurrences) {
                most_frequent_lengths_of_route.push_back(pair_length_number.first);
            }
        }
        return most_frequent_lengths_of_route;
    }

    double get_average_num_of_mistakes_for_real_routes() {
        if (vector_of_real_routes.empty()) {
            cerr << "Error in double get_average_num_of_mistakes_for_real_routes() : empty vector_of_real_routes\n";
            exit(1);
        }
        size_t num_of_mistakes_in_all_routes = 0;
        for (string_view s: vector_of_real_routes) {
            num_of_mistakes_in_all_routes += get_number_of_mistakes_in_route(s);
        }
        return double(num_of_mistakes_in_all_routes) / vector_of_real_routes.size();
    }

    size_t get_median_num_of_mistakes_for_real_routes() {
        if (vector_of_real_routes.empty()) {
            cerr << "Error in size_t get_median_num_of_mistakes_for_real_routes() : empty vector_of_real_routes\n";
            exit(1);
        }
        vector<size_t> nums_of_mistakes;
        for (string_view route: vector_of_real_routes) {
            nums_of_mistakes.push_back(get_number_of_mistakes_in_route(route));
        }
        sort(nums_of_mistakes.begin(), nums_of_mistakes.end());
        return nums_of_mistakes[nums_of_mistakes.size() / 2];
    }

    vector<size_t> get_most_frequent_num_of_mistakes_for_real_routes() {
        if (vector_of_real_routes.empty()) {
            cerr << "Error in vector<size_t> get_most_frequent_num_of_mistakes_for_real_routes() : empty vector_of_real_routes\n";
            exit(1);
        }
        unordered_map<size_t, size_t> num_of_mistakes_to_num_of_occurrences;
        int64_t max_num_of_occurrences = -1;
        for (string_view route: vector_of_real_routes) {
            int64_t cur = ++num_of_mistakes_to_num_of_occurrences[get_number_of_mistakes_in_route(route)];
            if (cur > max_num_of_occurrences) {
                max_num_of_occurrences = cur;
            }
        }
        vector<size_t> most_frequent_num_of_mistakes;
        for (auto &pair: num_of_mistakes_to_num_of_occurrences) {
            if (pair.second >= approximately_one * max_num_of_occurrences) {
                most_frequent_num_of_mistakes.push_back(pair.first);
            }
        }
        return most_frequent_num_of_mistakes;
    }

    void read_experiment_results(istream &in, ostream &out) {
        string string_for_nothing;
        getline(in, string_for_nothing);
        getline(in, string_for_nothing); // пропустить первые две строки файла с экспериментальными данными
        // теперь можно читать данные
        string line;
        while (getline(in, line)) {
            if (line.size() < 3) {
                continue;
            }
            stringstream ss(line);
            string rats_name, session_str, trial_str, route;
            if (typeOfMaze == type_of_maze::nikolskaya) {
                ss >> rats_name >> session_str >> trial_str >> route;
            } else if (typeOfMaze == type_of_maze::chelnok || typeOfMaze == type_of_maze::second_chelnok) {
                ss >> rats_name >> session_str >> trial_str >> route;
            } else {
                cerr << "Error in void read_experiment_results(istream &in, ostream &out) :: strange typeOfMaze\n";
                exit(1);
            }
            vector<vector<string>> &vv = rats_name_to_experiment_results[rats_name]; // vv = vector of vectors
            int session = stoi(session_str);
            int trial = stoi(trial_str);
            if (vv.size() == session) {
                vv.emplace_back();
            } else if (vv.size() < session) {
                vv.resize(session + 1, vector<string>());
            }
            vector<string> &v = vv[session]; // v = vector
            if (v.size() == trial) {
                v.emplace_back();
            } else if (v.size() < trial) {
                v.resize(trial + 1);
            }
            if (route_from_input_is_correct(route)) {
                if (route.back() == '+') {
                    route.pop_back();
                }
                v[trial] = route;
            }
        }
    }

#define research 1 // Используется в том случае, когда нужно найти
// оптимальные вероятности применения дискретных операторов

    void write_real_experiment_results_to_one_vector() {
        for (auto &pair: rats_name_to_experiment_results) {
            vector<vector<string>> &vv = pair.second;
            for (int cur_session = 1; cur_session < vv.size(); ++cur_session) {
                vector<string> &v = vv[cur_session];
                for (int cur_trial = 1; cur_trial < v.size(); ++cur_trial) {
                    string &cur_route = v[cur_trial];
                    if (cur_route.length() > 3) {
                        // Здесь можно задавать ограничения на номер серии и номер эксперимента внутри серии.
                        // Это нужно для рассмотрения обучения грызуна на различных этапах:
                        // начальном, промежуточном и конечном.
                        if (cur_session >= 10 && cur_trial >= 15) {
                            vector_of_real_routes.push_back(cur_route);
                        }
                    }
                }
            }
        }
    }

    void print_all_real_routes(ostream &o) {
        for (size_t i = 0; i < vector_of_real_routes.size(); ++i) {
            o << "\nRoute " << i + 1 << ":\n";
            o << vector_of_real_routes[i] << "\n";
        }
    }

    void set_optimal_probabilities(Rat &roger) {
        calculate_average_median_and_most_frequent_for_real_routes();

        construct_map_from_name_to_operator();

        double delimiter = 100;

        what_operators_are_optimized.push_back(operator_types::Operator_Random);
        what_operators_are_optimized.push_back(operator_types::Operator_Inversion);
        what_operators_are_optimized.push_back(operator_types::Operator_Symmetry);
        what_operators_are_optimized.push_back(operator_types::Operator_Circle);

        double cur_optimum = INF;
        vector<int> best_probs_for_operators = {0, 0, 0, 0};
        vector<int> best_probs_for_rel_dirs = {50, 22, 22, 6};
        double cur_value = INF;

        roger.set_probabilities_of_relative_dirs(best_probs_for_rel_dirs[0] / delimiter,
                                                 best_probs_for_rel_dirs[1] / delimiter,
                                                 best_probs_for_rel_dirs[2] / delimiter,
                                                 best_probs_for_rel_dirs[3] / delimiter);
        for (int prob_for_random = 99; prob_for_random > 0; prob_for_random -= 4) {
            // Для отслеживания человеком процесса исследования
            cout << "Probability for random operator = " << prob_for_random << " percent\n";
            for (int prob_for_inversion = 1;
                 prob_for_random + prob_for_inversion < 100 && prob_for_inversion <= 10; prob_for_inversion += 4) {
                for (int prob_for_symmetry = 1;
                     prob_for_random + prob_for_inversion + prob_for_symmetry < 100; prob_for_symmetry += 4) {
                    int prob_for_circle = 100 - prob_for_random - prob_for_inversion - prob_for_symmetry;
                    set_probabilities_for_operators({0, prob_for_random / delimiter, prob_for_inversion / delimiter,
                                                     prob_for_symmetry / delimiter,
                                                     prob_for_circle / delimiter});
                    conduct_series_of_experiments_memorizing_routes(2000, roger);
                    if (flag_has_to_stop) {
                        continue;
                    }
                    cur_value = calculate_optimization_function_clever_version();

                    if (cur_value < cur_optimum) {
                        cout << "Better option has been found!\n";
                        cout << cur_value << '\n';
                        cur_optimum = cur_value;
                        best_probs_for_operators[0] = prob_for_random;
                        best_probs_for_operators[1] = prob_for_inversion;
                        best_probs_for_operators[2] = prob_for_symmetry;
                        best_probs_for_operators[3] = prob_for_circle;
                        cout << "current answer:\nBest probabilities for operators are " << best_probs_for_operators[0]
                             << " and  "
                             << best_probs_for_operators[1] << " and  " << best_probs_for_operators[2] << "  and  "
                             << best_probs_for_operators[3] << " percent" << '\n';
                    }
                }
            }
        }

        if (best_probs_for_operators[0] + best_probs_for_operators[1] + best_probs_for_operators[2] +
            best_probs_for_operators[3] != 100 or
            cur_optimum == INF or
            best_probs_for_rel_dirs[0] + best_probs_for_rel_dirs[1] + best_probs_for_rel_dirs[2] +
            best_probs_for_rel_dirs[3] != 100) {
            cerr << "Error in void set_optimal_probabilities(Rat &roger)\n";
            exit(1);
        }
        roger.set_probabilities_of_relative_dirs(best_probs_for_rel_dirs[0] / delimiter,
                                                 best_probs_for_rel_dirs[1] / delimiter,
                                                 best_probs_for_rel_dirs[2] / delimiter,
                                                 best_probs_for_rel_dirs[3] / delimiter);
        set_probabilities_for_operators(
                {0.0, best_probs_for_operators[0] / delimiter, best_probs_for_operators[1] / delimiter,
                 best_probs_for_operators[2] / delimiter, best_probs_for_operators[3] / delimiter});
        conduct_series_of_experiments_memorizing_routes(2500, roger);
        cout << "FINAL ANSWER:\nBest probabilities for operators are " << best_probs_for_operators[0] << " and  "
             << best_probs_for_operators[1] << " and  " << best_probs_for_operators[2] << "  and  "
             << best_probs_for_operators[3] << " percent" << '\n';
        cout << "CHECKING:\noptimization function = " << calculate_optimization_function_simple_version() << '\n';
    }

    double calculate_optimization_function_clever_version() {
        if (average_len_of_real_routes == -1) {
            calculate_average_median_and_most_frequent_for_real_routes();
        }
        // average values
        if (average_len_of_real_routes < 0) {
            cerr << "Error :: average_len_of_real_routes < 0 in function double calculate_optimization_function()\n";
            exit(1);
        }
        average_len_of_generated_routes = get_average_length_of_generated_routes();
        double average_len_diff = abs(average_len_of_real_routes - average_len_of_generated_routes);
        cur_value_of_optimization_function = average_len_diff;
        if (cur_value_of_function_is_worse_than_optimum_value()) {
            return cur_value_of_optimization_function;
        }

        if (average_num_of_mistakes_for_real_routes < 0) {
            cerr
                    << "Error :: average_num_of_mistakes_for_real_routes < 0 in function double calculate_optimization_function()\n";
            exit(1);
        }
        average_num_of_mistakes_for_generated_routes = get_average_num_of_mistakes_for_generated_routes();
        double average_num_of_mistakes_diff = abs(
                average_num_of_mistakes_for_real_routes - average_num_of_mistakes_for_generated_routes);
        cur_value_of_optimization_function += average_num_of_mistakes_diff;
        if (cur_value_of_function_is_worse_than_optimum_value()) {
            return cur_value_of_optimization_function;
        }

        // medians
        if (median_len_of_real_routes < 0) {
            cerr << "Error :: median_len_of_real_routes < 0 in function double calculate_optimization_function()\n";
            exit(1);
        }
        median_len_of_generated_routes = get_median_length_of_generated_routes();
        long long median_len_diff = abs(median_len_of_real_routes - median_len_of_generated_routes);
        cur_value_of_optimization_function += pow(median_len_diff, 2.0);
        if (cur_value_of_function_is_worse_than_optimum_value()) {
            return cur_value_of_optimization_function;
        }

        if (median_num_of_mistakes_for_real_routes < 0) {
            cerr << "Error :: median_num_of_mistakes_for_real_routes < 0 in function double calculate_optimization_function()\n";
            exit(1);
        }
        median_num_of_mistakes_for_generated_routes = get_median_num_of_mistakes_for_generated_routes();
        long long median_num_of_mistakes_diff = abs(
                median_num_of_mistakes_for_real_routes - median_num_of_mistakes_for_generated_routes);
        cur_value_of_optimization_function += pow(median_num_of_mistakes_diff, 2.0);
        if (cur_value_of_function_is_worse_than_optimum_value()) {
            return cur_value_of_optimization_function;
        }

        // most_popular_values
        most_frequent_lens_of_generated_routes.clear();
        for (auto el: get_most_frequent_lengths_of_generated_routes()) {
            most_frequent_lens_of_generated_routes.push_back(el);
        }
        if (most_frequent_lens_of_generated_routes.empty() || most_frequent_lens_of_generated_routes[0] == -1) {
            cerr << "Error :: most_frequent_lens_of_generated_routes.empty() or its first element is negative in function double calculate_optimization_function()\n";
            exit(1);
        }
        cur_value_of_optimization_function += pow(
                assess_vectors_diff(most_frequent_lens_of_real_routes, most_frequent_lens_of_generated_routes), 0.1);
        if (cur_value_of_function_is_worse_than_optimum_value()) {
            return cur_value_of_optimization_function;
        }

        most_frequent_num_of_mistakes_for_generated_routes.clear();
        for (auto el: get_most_frequent_num_of_mistakes_for_generated_routes()) {
            most_frequent_num_of_mistakes_for_generated_routes.push_back(el);
        }
        if (most_frequent_num_of_mistakes_for_generated_routes.empty() ||
            most_frequent_num_of_mistakes_for_generated_routes[0] == -1) {
            cerr << "Error :: most_frequent_num_of_mistakes_for_generated_routes.empty() or its first element is negative in function double calculate_optimization_function()\n";
            exit(1);
        }
        cur_value_of_optimization_function += pow(assess_vectors_diff(most_frequent_num_of_mistakes_for_real_routes,
                                                                      most_frequent_num_of_mistakes_for_generated_routes),
                                                  0.1);
        return cur_value_of_optimization_function;
    }

    double calculate_optimization_function_simple_version() {
        if (average_len_of_real_routes == -1) {
            calculate_average_median_and_most_frequent_for_real_routes();
        }
        // average values
        if (average_len_of_real_routes < 0) {
            cerr << "Error :: average_len_of_real_routes < 0 in function double calculate_optimization_function()\n";
            exit(1);
        }
        average_len_of_generated_routes = get_average_length_of_generated_routes();
        double average_len_diff = abs(average_len_of_real_routes - average_len_of_generated_routes);
        cur_value_of_optimization_function = average_len_diff;
        if (average_num_of_mistakes_for_real_routes < 0) {
            cerr << "Error :: average_num_of_mistakes_for_real_routes < 0 in function double calculate_optimization_function()\n";
            exit(1);
        }
        average_num_of_mistakes_for_generated_routes = get_average_num_of_mistakes_for_generated_routes();
        double average_num_of_mistakes_diff = abs(
                average_num_of_mistakes_for_real_routes - average_num_of_mistakes_for_generated_routes);
        cur_value_of_optimization_function += average_num_of_mistakes_diff;
        // medians
        if (median_len_of_real_routes < 0) {
            cerr << "Error :: median_len_of_real_routes < 0 in function double calculate_optimization_function()\n";
            exit(1);
        }
        median_len_of_generated_routes = get_median_length_of_generated_routes();
        long long median_len_diff = abs(median_len_of_real_routes - median_len_of_generated_routes);
        cur_value_of_optimization_function += pow(median_len_diff, 2.0);
        if (median_num_of_mistakes_for_real_routes < 0) {
            cerr << "Error :: median_num_of_mistakes_for_real_routes < 0 in function double calculate_optimization_function()\n";
            exit(1);
        }
        median_num_of_mistakes_for_generated_routes = get_median_num_of_mistakes_for_generated_routes();
        long long median_num_of_mistakes_diff = abs(
                median_num_of_mistakes_for_real_routes - median_num_of_mistakes_for_generated_routes);
        cur_value_of_optimization_function += pow(median_num_of_mistakes_diff, 2.0);
        // most_popular_values
        most_frequent_lens_of_generated_routes.clear();
        for (auto el: get_most_frequent_lengths_of_generated_routes()) {
            most_frequent_lens_of_generated_routes.push_back(el);
        }
        if (most_frequent_lens_of_generated_routes.empty() || most_frequent_lens_of_generated_routes[0] == -1) {
            cerr << "Error :: most_frequent_lens_of_generated_routes.empty() or its first element is negative in function double calculate_optimization_function()\n";
            exit(1);
        }
        cur_value_of_optimization_function += pow(
                assess_vectors_diff(most_frequent_lens_of_real_routes, most_frequent_lens_of_generated_routes), 0.1);

        most_frequent_num_of_mistakes_for_generated_routes.clear();
        for (auto el: get_most_frequent_num_of_mistakes_for_generated_routes()) {
            most_frequent_num_of_mistakes_for_generated_routes.push_back(el);
        }
        if (most_frequent_num_of_mistakes_for_generated_routes.empty() ||
            most_frequent_num_of_mistakes_for_generated_routes[0] == -1) {
            cerr << "Error :: most_frequent_num_of_mistakes_for_generated_routes.empty() or its first element is negative in function double calculate_optimization_function()\n";
            exit(1);
        }
        cur_value_of_optimization_function += pow(assess_vectors_diff(most_frequent_num_of_mistakes_for_real_routes,
                                                                      most_frequent_num_of_mistakes_for_generated_routes),
                                                  0.1);
        return cur_value_of_optimization_function;
    }

    bool cur_value_of_function_is_worse_than_optimum_value() {
        return cur_value_of_optimization_function > optimum;
    }

    long long assess_vectors_diff(vector<long long> &v1, vector<long long> &v2) {
        unordered_set<long long> set1;
        unordered_set<long long> set2;
        for (auto el1: v1) {
            if (el1 == -1) {
                cerr << "Error: el1 == -1 in long long assess_vectors_diff\n";
                exit(1);
            }
            set1.insert(el1);
        }
        for (auto el2: v2) {
            if (el2 == -1) {
                cerr << "Error: el2 == -1 in long long assess_vectors_diff\n";
                exit(1);
            }
            set2.insert(el2);
        }
        long long diff = 0;
        for (auto el1: set1) {
            for (auto el2: set2) {
                if (set2.contains(el1) && set1.contains(el2)) {
                    continue;
                }
                diff += abs(el1 - el2);
            }
        }
        return diff;
    }

    void calculate_average_median_and_most_frequent_for_real_routes() {
        average_len_of_real_routes = get_average_length_of_real_route();
        median_len_of_real_routes = get_median_length_of_real_route();
        most_frequent_lens_of_real_routes.clear();
        for (auto el: get_most_frequent_lengths_of_real_route()) {
            most_frequent_lens_of_real_routes.push_back(el);
        }

        average_num_of_mistakes_for_real_routes = get_average_num_of_mistakes_for_real_routes();
        median_num_of_mistakes_for_real_routes = get_median_num_of_mistakes_for_real_routes();
        most_frequent_num_of_mistakes_for_real_routes.clear();
        for (auto el: get_most_frequent_num_of_mistakes_for_real_routes()) {
            most_frequent_num_of_mistakes_for_real_routes.push_back(el);
        }
        if (average_len_of_real_routes == -1 || median_len_of_real_routes == -1 ||
            most_frequent_lens_of_real_routes.empty() || most_frequent_lens_of_real_routes.front() == -1 ||
            average_num_of_mistakes_for_real_routes == -1 || median_num_of_mistakes_for_real_routes == -1 ||
            most_frequent_num_of_mistakes_for_real_routes.empty() ||
            most_frequent_num_of_mistakes_for_real_routes.front() == -1) {
            cerr << "Error in function void calculate_average_median_and_most_frequent_for_real_routes()!\n";
            exit(1);
        }
    }
};

void Operator_Random::apply(Maze &maze, Rat &rat) {
    maze.cur_generated_route.push_back(maze.process_random_move_of_rat(&rat));
}

void Operator_Random::set_probability(double probability) {
    prob = probability;
    cout << "Discrete operator RANDOM's probability has been set to " << prob << "\n";
}

void Operator_Inversion::apply(Maze &maze, Rat &rat) {
    // К текущему пути добавить последние len_of_path_to_inverse "русских" вершин в обратном порядке.
    // Начинаем с предпоследней вершины.

    int len_of_path_to_invert = round(my_generate_random(0.5 + eps, max_len_of_path_a_rat_can_invert + 0.5 - eps));
    int len_of_path_to_invert_saved = len_of_path_to_invert;

    string &path = maze.cur_generated_route;
    int len_of_path = int(path.length());
    int i = len_of_path - 2; // начинаем с предпоследней вершины
    if (i < 1) {
        return;
    }
    while (len_of_path_to_invert > 0 && i >= 0) {
        if (Is_Russian_Letter(path[i])) {
            --len_of_path_to_invert;
        }
        path.push_back(path[i]);
        --i;
    }

    if (path.length() >= 2) {
        rat.cur_cell_int = char_to_int[path[(int) (path.length()) - 1]];
        int from_num = char_to_int[path[(int) (path.length()) - 2]];
        int to_num = rat.cur_cell_int;
        rat.cur_direction = maze.get_abs_direction_from_cell_number_to_cell_number(from_num, to_num);
    }
}

void Operator_Inversion::set_probability(double probability) {
    prob = probability;
    cout << "Discrete operator INVERSION's probability has been set to " << prob << "\n";
}

void Operator_Symmetry::set_probability(double probability) {
    prob = probability;
}

abs_direction get_opposite_direction(abs_direction dir) {
    if (dir == abs_direction::left) {
        return abs_direction::right;
    }
    if (dir == abs_direction::right) {
        return abs_direction::left;
    }
    if (dir == abs_direction::up) {
        return abs_direction::down;
    }
    if (dir == abs_direction::down) {
        return abs_direction::up;
    }
    cerr << "Error in abs_direction get_opposite_direction(abs_direction dir)\n";
    exit(1);

}

void Operator_Symmetry::apply(Maze &maze, Rat &rat) {
    string &path = maze.cur_generated_route;
    abs_direction cur_dir = rat.cur_direction;

    if (maze.cells[rat.cur_cell_int].neighs[rat.cur_direction] == 0) {
#ifdef need_to_write_additional_info
        cout << "\nSymmetry operator hadn't been applied. The rat is facing a wall right now\n";
#endif
        return;
    }
    int i = int(path.length()) - 2;
    if (i < 0) {
        return;
    }
    while (i >= 0 && (cur_dir == maze.get_abs_direction_from_cell_number_to_cell_number(char_to_int[path[i]],
                                                                                        char_to_int[path[i + 1]]) ||
                      cur_dir == get_opposite_direction(
                              maze.get_abs_direction_from_cell_number_to_cell_number(char_to_int[path[i]],
                                                                                     char_to_int[path[i + 1]])))) {
        --i;
    }
    if (i < 0) {
        return;
    }
    abs_direction switched_direction = maze.get_abs_direction_from_cell_number_to_cell_number(char_to_int[path[i]],
                                                                                              char_to_int[path[i + 1]]);
    switched_direction = get_opposite_direction(switched_direction);

    // Грызун продолжает идти в этом направлении, пока не может повернуть в симметричную сторону.
    // При этом он должен сделать хотя бы одно продвижение в направлении cur_dir, если это возможно.
    // Только после этого грызун поворачивает
    if (maze.cells[rat.cur_cell_int].neighs[cur_dir] != 0) {
        int new_cell_int = maze.cells[char_to_int[path.back()]].neighs[cur_dir];
        rat.cur_direction = cur_dir;
        rat.cur_cell_int = new_cell_int;
        path.push_back(int_to_char[new_cell_int]);
    }

    while (maze.cells[rat.cur_cell_int].neighs[switched_direction] == 0) {
        if (maze.cells[rat.cur_cell_int].neighs[cur_dir] == 0) {
            return;
        }
        int new_cell_int = maze.cells[char_to_int[path.back()]].neighs[cur_dir];
        rat.cur_direction = cur_dir;
        rat.cur_cell_int = new_cell_int;
        path.push_back(int_to_char[new_cell_int]);
    }
    // process move in switched_dir;
    int new_cell_int = maze.cells[char_to_int[path.back()]].neighs[switched_direction];
    rat.cur_direction = switched_direction;
    rat.cur_cell_int = new_cell_int;
    path.push_back(int_to_char[new_cell_int]);
}

void Operator_Circle::set_probability(double probability) {
    prob = probability;
    cout << "Discrete operator CIRCLE's probability has been set to " << prob << "\n";
}

void Operator_Circle::apply(Maze &maze, Rat &rat) {
    if (maze.cells[rat.cur_cell_int].neighs[rat.cur_direction] == 0) {
#ifdef need_to_write_additional_info
        cout << "\nСircle operator hadn't been applied. We are in an impasse\n";
#endif
        return;
    }
    string &path = maze.cur_generated_route;
    // 1
    int neigh_num;
    while ((neigh_num = maze.cells[rat.cur_cell_int].neighs[rat.cur_direction]) != 0) {
        rat.cur_cell_int = neigh_num;
        path.push_back(int_to_char[neigh_num]);
    }
    abs_direction abs_dir_from_turning_right = determine_new_direction(rat.cur_direction, relative_direction::to_right);
    abs_direction abs_dir_from_turning_left = determine_new_direction(rat.cur_direction, relative_direction::to_left);
    relative_direction turning;
    if (maze.cells[rat.cur_cell_int].neighs[abs_dir_from_turning_right] == 0 &&
        maze.cells[rat.cur_cell_int].neighs[abs_dir_from_turning_left] == 0) {
#ifdef need_to_write_additional_info
        cout << "\nСircle operator is done. We are in an impasse\n";
#endif
        return;
    }
    if (maze.cells[rat.cur_cell_int].neighs[abs_dir_from_turning_right] == 0) {
        turning = relative_direction::to_left;
    } else if (maze.cells[rat.cur_cell_int].neighs[abs_dir_from_turning_left] == 0) {
        turning = relative_direction::to_right;
    } else {
        double randomly_generated_value = my_generate_random(0, 2);
        if (randomly_generated_value <= 1) {
            turning = relative_direction::to_left;
        } else {
            turning = relative_direction::to_right;
        }
    }
    // второй и третий процессы вида "поворот + движение в новом направлении до упора"
    for (int i = 0; i < 2; ++i) {
        rat.cur_direction = determine_new_direction(rat.cur_direction, turning);
        if (maze.cells[rat.cur_cell_int].neighs[rat.cur_direction] == 0) {
#ifdef need_to_write_additional_info
            cout << "\nСircle operator is done. We are in an impasse\n";
#endif
            return;
        }
        while ((neigh_num = maze.cells[rat.cur_cell_int].neighs[rat.cur_direction]) != 0) {
            rat.cur_cell_int = neigh_num;
            path.push_back(int_to_char[neigh_num]);
        }
    }
    rat.cur_direction = determine_new_direction(rat.cur_direction, turning);
    if (maze.cells[rat.cur_cell_int].neighs[rat.cur_direction] == 0) {
        rat.cur_direction = get_opposite_direction(rat.cur_direction);
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    Maze maze = Maze();

    // Для проведения виртуальных экспериментов следует редактировать эти строчки
    maze.typeOfMaze = type_of_maze::nikolskaya;
    bool caffeine = false;
    //

    string str_for_fin;
    if (maze.typeOfMaze == type_of_maze::nikolskaya) {
        str_for_fin = "Nikolskaya_maze_info.txt";
    } else if (maze.typeOfMaze == type_of_maze::chelnok) {
        str_for_fin = "chelnok_maze_info.txt";
    } else if (maze.typeOfMaze == type_of_maze::second_chelnok) {
        str_for_fin = "maze_second_chelnok_info.txt";
    }
    std::ifstream in_for_maze(str_for_fin);

    if (in_for_maze.is_open()) {
        cout << "File with maze description has been opened for input successfully!\n";
    } else {
        cerr << "ERROR! Cannot open file with maze description for input 1!\n";
        exit(1);
    }

    std::ofstream out;          // поток для записи
    out.open("output.txt"); // открываем файл для записи
    if (out.is_open()) {
        cout << "File for output opened successfully!\n";
    } else {
        cerr << "ERROR! Cannot open file for output!\n";
        exit(1);
    }

    maze.construct(in_for_maze, out);
    in_for_maze.close();
#ifdef print_all_info
    maze.print_info_about_cells_in_letters(out);
#endif

    string str_for_reading_experiments;
    if (maze.typeOfMaze == type_of_maze::nikolskaya) {
        cout << "Nikolskaya maze is being researched\n";
        str_for_reading_experiments = "Nikolskaya_experiments.txt";
    } else if (maze.typeOfMaze == type_of_maze::chelnok) {
        cout << "Chelnok is being researched\n";
        str_for_reading_experiments = "chelnok_experiments.txt";
    } else if (maze.typeOfMaze == type_of_maze::second_chelnok) {
        cout << "Second chelnok is being researched\n";
        if (caffeine)
            str_for_reading_experiments = "second_chelnok_experiments_with_caffeine.txt";
        else
            str_for_reading_experiments = "second_chelnok_experiments_without_caffeine.txt";
    }
    std::ifstream in_for_experiments(str_for_reading_experiments);
    if (in_for_experiments.is_open()) {
        cout << "File for input of experiment_results opened successfully!\n";
    } else {
        cerr << "ERROR! Cannot open file of experiment_results for input 2!\n";
        exit(1);
    }
    cout << "Started reading experiment results!\n";
    maze.read_experiment_results(in_for_experiments, out);
    cout << "Finished reading experiment results!\n";
    cout << "Started writing experiment results!\n";
    maze.write_real_experiment_results_to_one_vector();
    cout << "Finished writing experiment results!\n";

    Rat roger;

#ifdef research
    maze.set_optimal_probabilities(roger); // проверить эту функцию
#endif
#ifdef test_other
    maze.construct_map_from_name_to_operator();
    double delimiter = 100;
    roger.set_probabilities_of_relative_dirs(54 / delimiter, 20 / delimiter, 20 / delimiter, 6 / delimiter);
    maze.set_probabilities_for_operators({0.0, 85 / delimiter, 0 / delimiter, 0 / delimiter,  15 /
                                                                              delimiter});
    maze.conduct_series_of_experiments_memorizing_routes(3, roger);
#endif
    maze.print_statistical_data(out);
    out << "\n\n\n Печатаем реальные пути\n\n\n";
    maze.print_all_real_routes(out);
    out << "\n\n\n Теперь печатаем cгенерированные пути\n\n\n";
    maze.print_all_generated_routes(out);
    in_for_experiments.close();
    out.close();
    out << "\nDONE!\n";
    std::cout << "End of program" << "\n";
    return 0;
}
