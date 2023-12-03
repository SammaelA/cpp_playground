#include <iostream>
#include <fstream>
#include <assert.h>
#include <cstring>
#include <vector>
#include <map>
#include <algorithm>
#include <queue>
#include <math.h>
#include <set>

#pragma pack(push, 1)
struct Edge
{
  uint32_t v0;
  uint32_t v1;
  unsigned char weight;
};
#pragma pack(pop)

struct HuffmanNode
{
  uint32_t id;
  int freq;
  std::vector<bool> code;
  HuffmanNode *left = nullptr;
  HuffmanNode *right = nullptr;
};

struct HuffmanCode
{
  class compare
	{//a object funtion to set comparing rule of priority queue
	public:
		bool operator()(const HuffmanNode *c1, const HuffmanNode *c2) const
		{
			return c1->freq > c2->freq;
		}
	};
	std::priority_queue<HuffmanNode *, std::vector<HuffmanNode *>, compare> pq;		
  std::vector<HuffmanNode> nodes;
  HuffmanNode *root;
  void traverse(HuffmanNode *node, const std::vector<bool> &code)
  {
    if (node->left == NULL && node->right == NULL)
    {
      node->code = code;
      //std::cout<<"code for "<<node->id<<" ";
      //print_bin(code);
    }
    else
    {
      std::vector<bool> c1=code; c1.push_back(0);
      std::vector<bool> c2=code; c2.push_back(1);
      traverse(node->left, c1);
      traverse(node->right, c2);
    }
  }

  uint32_t binary_to_decimal(std::vector<bool>& in)
  {
    uint32_t result = 0;
    for (int i = 0; i < in.size(); i++)
      result = result * 2 + in[i];
    return result;
  }

  std::vector<bool> decimal_to_binary(uint32_t in)
  {
    std::vector<bool> v_rev;
    while (in)
    {
      v_rev.push_back(in%2);
      in /= 2;
    }
    std::vector<bool> v(v_rev.size());
    for (int i=0;i<v_rev.size();i++)
      v[i] = v_rev[v_rev.size()-i-1];
    return v;
  }

  void print_bin(const std::vector<bool> &v, const std::vector<int> &marks = {})
  {
    std::string s;
    int k = 0;
    for (int i=0;i<v.size();i++)
    {
      bool mark = false;
      for (auto &m : marks)
        if (m == i)
        {
          mark = true;
          break;
        }
      if (mark)
      {
        s+="|";
        k = 0;
      }
      else if (k%8==0)
        s+= " ";
      s += v[i] ? "1" : "0";
      k++;
    }
    std::cout<<s<<"\n";
  }

  void write_to_binary(uint32_t val, std::vector<bool> &binary)
  {
    for (int i=31;i>=0;i--)
      binary.push_back(val & (1u<<(uint32_t)i));
  }
  std::vector<bool> encode_all(std::vector<uint32_t> &all_values)
  {
    //calculate unique values and count them
    std::map<uint32_t, int> counts;
    std::map<uint32_t, int> table;
    for (uint32_t val : all_values)
      counts[val]++;
    
    nodes.reserve(2*counts.size());
    for (auto &p : counts)
    {
      nodes.emplace_back();
      nodes.back().id = p.first;
      nodes.back().freq = p.second;
      pq.push(&(nodes.back()));
      table[p.first] = nodes.size()-1; 
    }

    //create encoding table
    while (pq.size() > 1)
    {
      nodes.emplace_back();
      HuffmanNode *root = &(nodes.back());
      root->freq = 0;
      root->left = pq.top();
      root->freq += pq.top()->freq;
      pq.pop();
      root->right = pq.top();
      root->freq += pq.top()->freq;
      pq.pop();
      pq.push(root);
    }

    root = pq.top();
    traverse(root, {});

    //estimate compression ratio
    int prev_size = 8*all_values.size()*sizeof(uint32_t);
    int codes_size = 0;
    int code_max_bits = 0;
    for (auto &n : nodes)
    {
      if (n.left == nullptr && n.right == nullptr)
      {
        codes_size += n.code.size()*n.freq;
        code_max_bits = std::max(code_max_bits, (int)n.code.size());
      }
    }
    int table_size = counts.size()*(8*sizeof(uint32_t) + code_max_bits);
    std::cout<<"max bit per symbol "<<code_max_bits<<"\n";
    std::cout<<"expected compression ratio "<<(float)prev_size/(table_size+codes_size)<<"\n";
    std::cout<<"prev size "<<prev_size<<", cur size "<<table_size<<"+"<<codes_size<<"\n";

    //encode table and message size
    std::vector<bool> full_code;
    std::vector<int> marks;
    write_to_binary(table.size(), full_code); marks.push_back(full_code.size());
    write_to_binary(all_values.size(), full_code); marks.push_back(full_code.size());
    write_to_binary(code_max_bits, full_code); marks.push_back(full_code.size());

    //encode the table
    full_code.reserve(full_code.size() + codes_size);
    for (auto &p : table)
    {
      //{(value, extended_code)}
      write_to_binary(p.first, full_code); marks.push_back(full_code.size());
      auto code = nodes[p.second].code;
      for (int i=code.size();i<code_max_bits;i++)
        code.push_back(0);
      full_code.insert(full_code.end(), code.begin(), code.end()); marks.push_back(full_code.size());
    }

    //encode the message
    for (uint32_t &val : all_values)
    {
      auto code = nodes[table[val]].code;
      full_code.insert(full_code.end(), code.begin(), code.end()); marks.push_back(full_code.size());    
    }

    //print_bin(full_code, marks);
    return full_code;
  }

  uint32_t read_from_binary(const std::vector<bool> &binary, int *index)
  {
    uint32_t res = 0;
    for (int i=31;i>=0;i--)
    {
      res = res | ((uint32_t)(binary[*index])<<(uint32_t)i);
      (*index)++;
    }
    return res;
  }

  std::vector<uint32_t> decode_all(const std::vector<bool> &binary)
  {
    //decode table and message size
    int index = 0;
    int table_size = read_from_binary(binary, &index);
    int message_size = read_from_binary(binary, &index);
    int code_max_bits = read_from_binary(binary, &index);

    //decode table and build decoding tree
    nodes.reserve(code_max_bits*table_size);//just want to make sure no reallocations happen :)
    nodes.emplace_back();
    root = &(nodes.back());

    for (int i=0;i<table_size;i++)
    {
      //read value and fixed_size(!) code
      uint32_t value = read_from_binary(binary, &index);
      std::vector<bool> code(code_max_bits);
      for (int j=0;j<code_max_bits;j++)
      {
        code[j] = binary[index];
        index++;
      }
      //print_bin(code);

      //put it into node tree, create nodes if necessary
      HuffmanNode *cur_node = root;
      for (int code_index = 0; code_index<code_max_bits;code_index++)
      {
        if (code[code_index] == 0)
        {
          if (cur_node->left == nullptr)
          {
            nodes.emplace_back();
            cur_node->left = &(nodes.back());
          }
          cur_node = cur_node->left;
        }
        else
        {
          if (cur_node->right == nullptr)
          {
            nodes.emplace_back();
            cur_node->right = &(nodes.back());
          }
          cur_node = cur_node->right;
        }
      }
      cur_node->id = value;
    }

    //trim decoding tree - removing nodes with one child
    bool one_child_nodes = true;
    while (one_child_nodes)
    {
      one_child_nodes = false;
      for (auto &n : nodes)
      {
        HuffmanNode *one_child = nullptr;
        if (n.left && n.right == nullptr)
          one_child = n.left;
        else if (n.left == nullptr && n.right)
          one_child = n.right;
        
        if (one_child)
        {
          one_child_nodes = true;
          n.left = one_child->left;
          n.right = one_child->right;
          n.id = one_child->id;
          one_child->left = nullptr;
          one_child->right = nullptr;
        }
      }
    }
    traverse(root, {});

    //read values until the end of the code
    std::vector<uint32_t> values;
    const HuffmanNode *cur_node = root;
    while (index < binary.size())
    {
      if (cur_node->left == nullptr && cur_node->right == nullptr)
      {
        values.push_back(cur_node->id);
        //std::cout<<"decoded "<<cur_node->id<<"\n";
        cur_node = root;
      }
      else 
      {
        if (binary[index] == 0)
          cur_node = cur_node->left;
        else /*if (binary[index] == 1)*/
          cur_node = cur_node->right;
        index++;
      }
    }
    values.push_back(cur_node->id);
    //std::cout<<"decoded "<<cur_node->id<<"\n";

    return values;
  }
};

void to_normalized_form(std::vector<Edge> &edges)
{
  for (auto &e : edges)
    if (e.v0 > e.v1)
      std::swap(e.v0, e.v1);
  std::sort(edges.begin(), edges.end(), [](const Edge &a, const Edge &b)->bool 
  {
    if (a.v0 != b.v0)
      return a.v0 < b.v0;
    else if (a.v1 != b.v1)
      return a.v1<b.v1;
    else 
      return a.weight<b.weight;
  });
}

std::vector<Edge> get_edges(const char *input_path)
{
  std::ifstream in(input_path);
  assert(in.is_open());

  std::vector<Edge> edges;
  uint32_t v0, v1, w;
  while (in)
  {
    in >> v0;
    in >> v1;
    in >> w;
    if (in)
      edges.push_back({v0,v1,(unsigned char)w});
  }
  to_normalized_form(edges);
  in.close();
  return edges;
}

void write_edges(const char *output_path, const std::vector<Edge> &edges)
{
  std::ofstream out(output_path);
  assert(out.is_open());
  for (const Edge &edge : edges)
    out << edge.v0 <<"\t"<< edge.v1 <<"\t"<< (uint32_t)edge.weight<<"\n";
  out.close();
}

void serialize(const char *input_path, const char *output_path)
{
  auto edges = get_edges(input_path);
  size_t size = edges.size();
  std::ofstream out(output_path, std::ios_base::binary);
  assert(out.is_open());
  out.write(reinterpret_cast<char*>(&size), sizeof(size_t));
  out.write(reinterpret_cast<char*>(edges.data()), sizeof(Edge)*edges.size());

  std::cout<<size<<" edges\n";
  int byte_size = sizeof(size_t) + size*sizeof(Edge);
  std::cout<<"total "<< (float)byte_size/size << " bytes/edge\n";

  out.close();
}

void deserialize(const char *input_path, const char *output_path)
{
  std::ifstream in(input_path);
  assert(in.is_open());

  size_t count = 0;
  std::vector<Edge> edges;
  in.read(reinterpret_cast<char*>(&count), sizeof(count));
  edges.resize(count);
  in.read(reinterpret_cast<char*>(edges.data()), sizeof(Edge)*edges.size());
  in.close();

  write_edges(output_path, edges);
}

void serialize_2(const char *input_path, const char *output_path)
{
  auto edges = get_edges(input_path);
  size_t size = edges.size();
  std::ofstream out(output_path, std::ios_base::binary);
  assert(out.is_open());

  //[v0,N,v1_1,w1...,v1_N,w1]
  int cur_start = 0;
  int byte_size = sizeof(size_t);
  std::vector<uint32_t> all_values;
  out.write(reinterpret_cast<char*>(&size), sizeof(size_t));  all_values.push_back(size);
  for (int i=0;i<=size;i++)
  {
    if (i == size || edges[i].v0 != edges[cur_start].v0)
    {
      //put to binary
      out.write(reinterpret_cast<char*>(&(edges[cur_start].v0)), sizeof(uint32_t));  all_values.push_back(edges[cur_start].v0);
      int count = i-cur_start;
      out.write(reinterpret_cast<char*>(&count), sizeof(int));  all_values.push_back(count);
      for (int j=cur_start;j<i;j++)
      {
        out.write(reinterpret_cast<char*>(&(edges[j].v1)), sizeof(uint32_t));  all_values.push_back(edges[j].v1);
        out.write(reinterpret_cast<char*>(&(edges[j].weight)), sizeof(unsigned char));  all_values.push_back(edges[j].weight);
      }
      byte_size += sizeof(int) + (count+1)*sizeof(uint32_t) + count*sizeof(unsigned char);
      if (i!=size)
        cur_start = i;
    }
  }
  //out.write(reinterpret_cast<char*>(&size), sizeof(size_t));
  //out.write(reinterpret_cast<char*>(edges.data()), sizeof(Edge)*edges.size());

  std::cout<<size<<" edges\n";
  std::cout<<"total "<< (float)byte_size/size << " bytes/edge\n";

  out.close();
}

void deserialize_2(const char *input_path, const char *output_path)
{
  std::ifstream in(input_path);
  assert(in.is_open());

  size_t size = 0;
  std::vector<Edge> edges;
  in.read(reinterpret_cast<char*>(&size), sizeof(size_t));
  edges.reserve(size);
  while (edges.size() < size)
  {
    uint32_t v0;
    int count;
    in.read(reinterpret_cast<char*>(&v0), sizeof(uint32_t));
    in.read(reinterpret_cast<char*>(&count), sizeof(int));
    for (int i=0;i<count;i++)
    {
      edges.emplace_back();
      edges.back().v0 = v0;
      in.read(reinterpret_cast<char*>(&(edges.back().v1)), sizeof(uint32_t));
      in.read(reinterpret_cast<char*>(&(edges.back().weight)), sizeof(unsigned char));
    }
  }
  //in.read(reinterpret_cast<char*>(edges.data()), sizeof(Edge)*edges.size());
  in.close();

  write_edges(output_path, edges);
}

void write_bits_to_binary(const std::vector<bool> &bits, const char *output_path)
{
  std::ofstream out(output_path, std::ios_base::binary);
  assert(out.is_open());

  static constexpr int ELEM_SIZE = 32;
  uint32_t bit_size = bits.size();
  std::vector<uint32_t> data((bit_size+ELEM_SIZE-1)/ELEM_SIZE);
  for (uint32_t i=0;i<bit_size;i++)
    data[i/ELEM_SIZE] |= (uint32_t)bits[i] << (i%ELEM_SIZE);
  out.write(reinterpret_cast<char*>(&bit_size), sizeof(uint32_t));
  out.write(reinterpret_cast<char*>(data.data()), sizeof(uint32_t)*data.size());

  out.close();
}

std::vector<bool> read_bits_from_binary(const char *input_path)
{
  std::ifstream in(input_path);
  assert(in.is_open());
  uint32_t bit_size;
  in.read(reinterpret_cast<char*>(&bit_size), sizeof(uint32_t));
  static constexpr int ELEM_SIZE = 32;
  std::vector<uint32_t> data((bit_size+ELEM_SIZE-1)/ELEM_SIZE);
  in.read(reinterpret_cast<char*>(data.data()), sizeof(uint32_t)*data.size());

  std::vector<bool> bits(bit_size);
  for (uint32_t i=0;i<bit_size;i++)
    bits[i] = data[i/ELEM_SIZE] & (1u << (i%ELEM_SIZE));
  
  return bits;
} 

void serialize_3(const char *input_path, const char *output_path)
{
  auto edges = get_edges(input_path);
  size_t size = edges.size();

  //[v0,N,v1_1,w1...,v1_N,w1]
  int cur_start = 0;
  int byte_size = sizeof(size_t);
  std::vector<uint32_t> all_values;
  all_values.push_back(size);
  for (int i=0;i<=size;i++)
  {
    if (i == size || edges[i].v0 != edges[cur_start].v0)
    {
      //put to binary
      all_values.push_back(edges[cur_start].v0);
      int count = i-cur_start;
      all_values.push_back(count);
      for (int j=cur_start;j<i;j++)
      {
        all_values.push_back(edges[j].v1);
        all_values.push_back(edges[j].weight);
      }
      byte_size += sizeof(int) + (count+1)*sizeof(uint32_t) + count*sizeof(unsigned char);
      if (i!=size)
        cur_start = i;
    }
  }

  HuffmanCode Hcode;
  auto bits = Hcode.encode_all(all_values);
  write_bits_to_binary(bits, output_path);

  std::cout<<size<<" edges\n";
  std::cout<<"initially "<< (float)byte_size/size << " bytes/edge\n";
  std::cout<<"after encoding "<< (float)bits.size()/(8*size) << " bytes/edge\n";
}

void deserialize_3(const char *input_path, const char *output_path)
{
  auto bits = read_bits_from_binary(input_path);
  HuffmanCode Hcode;
  std::vector<uint32_t> all_values = Hcode.decode_all(bits);
  int index = 0;
  #define NEXT (all_values[index++]) 

  size_t size = NEXT;
  std::vector<Edge> edges;
  edges.reserve(size);
  while (edges.size() < size)
  {
    uint32_t v0 = NEXT;
    int count = NEXT;
    for (int i=0;i<count;i++)
    {
      edges.emplace_back();
      edges.back().v0 = v0;
      edges.back().v1 = NEXT;
      edges.back().weight = NEXT;
    }
  }
  write_edges(output_path, edges);
}

void verify(const char *file1, const char *file2)
{
  auto edges1 = get_edges(file1);
  auto edges2 = get_edges(file2);
  assert(edges1.size() == edges2.size());
  bool eq = true;
  for (int i=0;i<edges1.size();i++)
  {
    if (edges1[i].v0 != edges2[i].v0 || edges1[i].v1 != edges2[i].v1 || edges1[i].weight != edges2[i].weight)
    {
      eq = false;
      std::cout<<i<<" diff ["<<edges1[i].v0<<","<<edges1[i].v1<<","<<(int)edges1[i].weight<<"] and ["<<edges2[i].v0<<","<<edges2[i].v1<<","<<(int)edges2[i].weight<<"\n";
    }
  }
  if (eq)
    std::cout<<"Verify: SUCCESS\n";
  else
    std::cout<<"Verify: FAIL\n";
}

void analyze(const char *input_path)
{
  auto edges = get_edges(input_path);
  size_t size = edges.size();
  std::map<uint32_t, int> counts;
  for (auto &edge : edges)
  {
    counts[edge.v0]++;
    counts[edge.v1]++;
  }
  std::vector<std::pair<uint32_t, int>> top_counts;
  top_counts.reserve(counts.size());
  for (auto &p : counts)
    top_counts.push_back(p);
  std::sort(top_counts.begin(), top_counts.end(), [&](std::pair<uint32_t, int>& a, std::pair<uint32_t, int>& b) -> bool {return a.second > b.second;});
  
  //for (auto &p : top_counts)
  //  std::cout<<"cnt["<<p.first<<"]="<<p.second<<"\n";
  
  std::map<int, int> v_powers;
  for (auto &p : top_counts)
    v_powers[p.second]++;
  
  std::cout<<"density "<<(float)size/counts.size()<<"\n";

  for (auto &p : v_powers)
    std::cout<<"power["<<p.first<<"]="<<p.second<<"\n";
}

double rand(double from, double to)
{
  return from + ((double)rand()/RAND_MAX)*(to-from);
}
void generate_graph(int size, const char *output_path, int min_power = 5, double power_distr = log(6)/log(2))
{
  struct Vertex
  {
    uint32_t id;
    int max_edges;
    int left_edges;
  };

  constexpr int MAX_POWER = 2048;
  double chances[MAX_POWER];
  for (int i=0;i<MAX_POWER;i++)
  {
    double v = 1/pow(min_power+i, power_distr);
    chances[i] = (i==0 ? 0 : chances[i-1])+v;
  }

  std::vector<Vertex> vertices(size);
  std::set<uint32_t> occupied_ids;
  int total_edges = 0;
  for (auto &v : vertices)
  {
    //create unique id
    uint32_t id = 0;
    while (occupied_ids.find(id) != occupied_ids.end())
      id = 2u*((uint32_t)rand()) + rand()%2;
    v.id = id;
    occupied_ids.emplace(id);

    //choose number of edges
    double rnd = rand(0, chances[MAX_POWER-1]);
    int i = 0;
    while (rnd>=chances[i] && i < MAX_POWER-1)
      i++;
    v.max_edges = min_power+i;
    v.left_edges = v.max_edges;
    total_edges += v.max_edges;
    std::cout<<"vertex "<<v.id<<" created\n";
  }

  if (total_edges%2)
  {
    vertices[0].max_edges++;
    vertices[0].left_edges++;
    total_edges++;
  }
  total_edges /= 2;

  //create edges by finding two random connections
  std::vector<Edge> edges(total_edges);
  int connections_left = 2*total_edges;
  for (auto &e : edges)
  {
    int c_1 = rand() % connections_left;
    int i1 = 0;
    while (c_1>vertices[i1].left_edges)
    {
      c_1 -=vertices[i1].left_edges;
      i1++;
    }
    vertices[i1].left_edges--;
    connections_left--;

    int c_2 = rand() % connections_left;
    int i2 = 0;
    while (c_2>vertices[i2].left_edges)
    {
      c_2 -=vertices[i2].left_edges;
      i2++;
    }
    vertices[i2].left_edges--;
    connections_left--;

    e.v0 = vertices[i1].id;
    e.v1 = vertices[i2].id;
    e.weight = rand()%256;
    std::cout<<connections_left<<" edge "<<e.v0<<" "<<e.v1<<" created\n";
  }

  write_edges(output_path, edges);
}

int main(int argc, char **argv)
{
  //HuffmanCode Hcode;
  //std::vector<uint32_t> uvec = {4,4,4,4,16, 32};
  //HuffmanCode Hdecode;
  //auto bits = Hcode.encode_all(uvec);
  //write_bits_to_binary(bits,"bits.bin");
  //auto new_bits = read_bits_from_binary("bits.bin");
  //std::vector<uint32_t> new_uvec = Hdecode.decode_all(new_bits);
  //return 0;
  //std::vector<uint32_t> uvec = {7,7,7,7,77,77,77,777,777,666,666,8888};
  //HuffmanCode Hcode(uvec);
  //return 0;
  if (strcmp(argv[1],"-s") == 0)
  {
    assert(argc == 6);
    serialize_3(argv[3], argv[5]);
  }
  else if (strcmp(argv[1],"-d") == 0)
  {
    assert(argc == 6);
    deserialize_3(argv[3], argv[5]);
  }
  else if (strcmp(argv[1],"-v") == 0)
  {
    assert(argc == 4);
    verify(argv[2], argv[3]);
  }
  else if (strcmp(argv[1],"-a") == 0)
  {
    assert(argc == 3);
    analyze(argv[2]);
  }
  else if (strcmp(argv[1],"-g") == 0)
  {
    assert(argc == 3);
    generate_graph(1000000, argv[2]);    
  }
}
