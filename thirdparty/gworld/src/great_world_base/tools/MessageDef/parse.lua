require("base")
local define_file_name = arg[1]
require(define_file_name)

local BaseTypeMap = {}
for k, v in pairs(__BaseType) do
  BaseTypeMap[v] = k
end

local ClassTypeMap = {}
for k, v in pairs(__ClassType) do
  ClassTypeMap[v] = k
end

local class_template = [[
class %s {
 private:
  // member
  %s 
 public:
  %s() = default;
  %s(const %s&) = default;
  %s& operator=(const %s&) = default;
  // constructor
  %s
  // move constructor
  %s
  // serialize
  %s
  // getter and setter
  %s
};
]]
local file_template = [[
// 头文件
%s
%s

%s

%s
]]


local space_str = "  "

function parse_mem(members)
  local ret = {}
  for _, mem in ipairs(members) do 
    local mem_name, mem_type, final_name = mem.name, mem.type, mem.final_name
    table.insert(ret, mem_type)
    table.insert(ret, " ")
    table.insert(ret, final_name)
    table.insert(ret, ";\n")
    table.insert(ret, space_str)
  end
  return table.concat(ret)
end

function parse_constructor(class_name, members)
  local ret = {}
  table.insert(ret, class_name)
  table.insert(ret, "(")
  for i, mem in ipairs(members) do 
    local mem_name, mem_type, final_name = mem.name, mem.type, mem.final_name
    table.insert(ret, mem_type)
    table.insert(ret, " ")
    table.insert(ret, mem_name)
    if members[i+1] then
      table.insert(ret, ", ")
    end
  end
  table.insert(ret, ") : ")
  for i, mem in ipairs(members) do 
    local mem_name, mem_type, final_name = mem.name, mem.type, mem.final_name
    table.insert(ret, final_name)
    table.insert(ret, "(")
    table.insert(ret, mem_name)
    table.insert(ret, ")")
    if members[i+1] then
      table.insert(ret, ", ")
    end
  end
  table.insert(ret, " { }")
  return table.concat(ret)
end

function parse_move_constructor(class_name, members)
  local ret = {}
  table.insert(ret, class_name)
  table.insert(ret, "(")
  table.insert(ret, class_name)
  table.insert(ret, "&& other")
  table.insert(ret, ") : ")
  for i, mem in ipairs(members) do 
    local mem_name, mem_type, final_name = mem.name, mem.type, mem.final_name
    table.insert(ret, final_name)
    table.insert(ret, "(std::move(other.")
    table.insert(ret, final_name)
    table.insert(ret, "))")
    if members[i+1] then
      table.insert(ret, ", ")
    end
  end
  table.insert(ret, " { }")
  return table.concat(ret)
end

-- 处理序列化
local parse_o_router
local parse_i_router
function parse_base_o_serialize(mem_type, v_name)
  return "*os << " .. v_name .. ";"
end
function parse_base_i_serialize(mem_type, v_name)
  return "*is >> " .. v_name .. ";"
end

function parse_mydef_o_serialize(mem_type, v_name)
  return v_name .. ".ToString(os);"
end
function parse_mydef_i_serialize(mem_type, v_name)
  return v_name .. ".FromString(is);"
end


local vector_o_template = [[*os << %s.size(); for (auto& v:%s) { %s }]]
local vector_i_template = [[std::size_t %s_i; *is >> %s_i; for (std::size_t i = 0; i!= %s_i ; i++) { %s tmp; %s; %s.push_back(std::move(tmp)); }]]
function parse_vector_o_serialize(mem_type, v_name)
  local _, _, sub_type = string.find(mem_type, "GWVector<(%a+)>")
  assert(sub_type, "vector type error")
  return string.format(vector_o_template, v_name, v_name, parse_o_router(sub_type, "v"))
end
function parse_vector_i_serialize(mem_type, v_name)
  local _, _, sub_type = string.find(mem_type, "GWVector<(%a+)>")
  assert(sub_type, "vector type error")
  return string.format(vector_i_template, v_name, v_name, v_name, sub_type, parse_i_router(sub_type, "tmp"), v_name)
end

function parse_o_router(mem_type, v_name)
  if string.find(mem_type, "GWVector") then
    return parse_vector_o_serialize(mem_type, v_name)
  elseif not BaseTypeMap[mem_type] and not ClassTypeMap[mem_type] then
    return parse_mydef_o_serialize(mem_type, v_name)
  else
    return parse_base_o_serialize(mem_type, v_name)
  end
end
function parse_i_router(mem_type, v_name)
  if string.find(mem_type, "GWVector") then
    return parse_vector_i_serialize(mem_type, v_name)
  elseif not BaseTypeMap[mem_type] and not ClassTypeMap[mem_type] then
    return parse_mydef_i_serialize(mem_type, v_name)
  else
    return parse_base_i_serialize(mem_type, v_name)
  end
end

function parse_serialize(members)
  local ret = {}
  table.insert(ret, "bool ToString(IMemoryOStream* os) const {\n")
  for i, mem in ipairs(members) do 
    local mem_name, mem_type, final_name = mem.name, mem.type, mem.final_name
    table.insert(ret, space_str)
    table.insert(ret, space_str)
    table.insert(ret, parse_o_router(mem_type, final_name))
    table.insert(ret, "\n")
  end
  table.insert(ret, space_str)
  table.insert(ret, space_str)
  table.insert(ret, "return true;\n")
  table.insert(ret, space_str)
  table.insert(ret, "}\n")

  table.insert(ret, space_str)
  table.insert(ret, "bool FromString(IMemoryIStream* is) {\n")
  for i, mem in ipairs(members) do 
    local mem_name, mem_type, final_name = mem.name, mem.type, mem.final_name
    table.insert(ret, space_str)
    table.insert(ret, space_str)
    table.insert(ret, parse_i_router(mem_type, final_name))
    table.insert(ret, "\n")
  end
  table.insert(ret, space_str)
  table.insert(ret, space_str)
  table.insert(ret, "return true;\n")
  table.insert(ret, space_str)
  table.insert(ret, "}\n")
  return table.concat(ret)
end

local template_getter = [[
inline %s %s() const { return %s; }
  inline %s& mutable_%s() { return %s; }
]]
local template_setter = [[
inline void set_%s(const %s& tmp) { %s = tmp; }
  inline void set_%s(%s&& tmp) { %s = std::move(tmp); }
]]
local template_vector_add = [[
inline void add_%s(%s&& tmp) { %s.emplace_back(std::move(tmp)); }
  inline void add_%s(const %s& tmp) { %s.emplace_back(tmp); }
  inline std::size_t %s_size() { return %s.size(); }
]]
function parse_getter_setter(members)
  local ret = {}
  table.insert(ret, "\n")
  for i, mem in ipairs(members) do 
    local mem_name, mem_type, final_name = mem.name, mem.type, mem.final_name
    table.insert(ret, space_str)
    local new_mem_type = mem_type 
    if not BaseTypeMap[mem_type] then
      new_mem_type = "const " .. mem_type .. "&"
    end
    table.insert(ret, string.format(template_getter, new_mem_type, mem_name, final_name, mem_type, mem_name, final_name))
  end
  table.insert(ret, "\n")
  for i, mem in ipairs(members) do 
    local mem_name, mem_type, final_name = mem.name, mem.type, mem.final_name
    table.insert(ret, space_str)
    table.insert(ret, string.format(template_setter, mem_name, mem_type, final_name, mem_name, mem_type, final_name))
  end
  table.insert(ret, "\n")
  for i, mem in ipairs(members) do 
    local mem_name, mem_type, final_name = mem.name, mem.type, mem.final_name
    if string.find(mem_type, "GWVector") then
      local _, _, sub_type = string.find(mem_type, "GWVector<(%a+)>")
      table.insert(ret, space_str)
      table.insert(ret, string.format(template_vector_add, mem_name, sub_type, final_name, mem_name, sub_type, final_name, mem_name, final_name))
    end
  end
  return table.concat(ret)
end

-- 各个属性的 getter/setter
-- 移动构造函数
-- 序列化
-- 反序列化
function parse_class(class_name, members) 
  for i, mem in ipairs(members) do 
    mem.final_name = mem.name .. "_"
  end
  local class_str = string.format(class_template,
    class_name,
    parse_mem(members),
    class_name, class_name, class_name, class_name, class_name,
    parse_constructor(class_name, members),
    parse_move_constructor(class_name, members),
    parse_serialize(members),
    parse_getter_setter(members)
  )
  return class_str
end
local classes_ret = {}
for i_, data in ipairs(__Classes) do
  table.insert(classes_ret, parse_class(data.name, data.members))
end
local classes_str = table.concat(classes_ret, "\n")

local final_str = string.format(file_template, __Header, __Namespace_Begin, classes_str, __Namespace_End)
local file = io.open(__OutputFileName, "w")
io.output(file)
io.write(final_str)
io.close(file)
