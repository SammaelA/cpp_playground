#include <iostream>
#include <fstream>
#include <assert.h>
#include <cstring>
#include <vector>
#include <map>
#include <algorithm>
#include <queue>

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

  void print_bin(const std::vector<bool> &v)
  {
    std::string s;
    for (bool val : v)
      s += val ? "1" : "0";
    std::cout<<s<<"\n";
  }

  HuffmanCode(std::vector<uint32_t> &all_values)
  {
    std::map<uint32_t, int> counts;
    for (uint32_t val : all_values)
      counts[val]++;
    
    nodes.reserve(2*counts.size());
    for (auto &p : counts)
    {
      nodes.emplace_back();
      nodes.back().id = p.first;
      nodes.back().freq = p.second;
      pq.push(&(nodes.back()));
    }

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

    int prev_size = 8*all_values.size()*sizeof(uint32_t);
    int table_size = 8*counts.size()*sizeof(uint32_t);
    int codes_size = 0;
    for (auto &n : nodes)
    {
      if (n.left == nullptr && n.right == nullptr)
        codes_size += n.code.size();
    }
    std::cout<<"expected compression ratio "<<(float)prev_size/(table_size+codes_size)<<"\n";
    std::cout<<"prev size "<<prev_size<<", cur size "<<table_size<<"+"<<codes_size<<"\n";
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
    else
      return a.v1<b.v1;
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
  /*
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
  */
  //for (auto &p : top_counts)
  //  std::cout<<"cnt["<<p.first<<"]="<<p.second<<"\n";
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

  std::ofstream out(output_path);
  assert(out.is_open());
  for (Edge &edge : edges)
    out << edge.v0 <<"\t"<< edge.v1 <<"\t"<< (uint32_t)edge.weight<<"\n";
  out.close();
  in.close();
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

  HuffmanCode Hcode(all_values);
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

  std::ofstream out(output_path);
  assert(out.is_open());
  for (Edge &edge : edges)
    out << edge.v0 <<"\t"<< edge.v1 <<"\t"<< (uint32_t)edge.weight<<"\n";
  out.close();
  in.close();
}

struct Bitfield
{
  Bitfield() = default;
  Bitfield(const std::vector<bool> &bits)
  {
    bit_size = bits.size();
    data.resize((bit_size+ELEM_SIZE-1)/ELEM_SIZE);
    for (int i=0;i<bit_size;i++)
      data[i/ELEM_SIZE] |= (uint64_t)bits[i] << (i%ELEM_SIZE);
  }

 static constexpr int ELEM_SIZE = 64;
 std::vector<uint64_t> data;
 int bit_size = 0;
};

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
      break;
    }
  }
  if (eq)
    std::cout<<"Verify: SUCCESS\n";
  else
    std::cout<<"Verify: FAIL\n";
}

int main(int argc, char **argv)
{
  //std::vector<uint32_t> uvec = {7,7,7,7,77,77,77,777,777,666,666,8888};
  //HuffmanCode Hcode(uvec);
  //return 0;
  if (strcmp(argv[1],"-s") == 0)
  {
    assert(argc == 6);
    serialize_2(argv[3], argv[5]);
  }
  else if (strcmp(argv[1],"-d") == 0)
  {
    assert(argc == 6);
    deserialize_2(argv[3], argv[5]);
  }
  else if (strcmp(argv[1],"-v") == 0)
  {
    assert(argc == 4);
    verify(argv[2], argv[3]);
  }
}
