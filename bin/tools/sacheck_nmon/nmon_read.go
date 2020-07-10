package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	"github.com/pkg/term"
	"io/ioutil"
	"os"
	"strconv"
	"strings"
)

//全局变量
var filename = ""                      //文件名称
var item = make(map[string]int, 500)   //检查项
var item_title = make([][]string, 1000) //检查项标题
var c_k chan int
var b_cpu = true
var search = "" //查找字符串

var t_cpu_num = 10

//检查错误
func checkErr(err error) {
	if err != nil {
		panic(err)
	}
}

//读取时间点
func read_time(content []byte, time string) int {

	index := strings.Index(string(content), "\nZZZZ") //找到时间行
	if index == -1 {
		return -1
	}

	index = index + 1

	if time == "" { //时间为空直接返回第一次找到的
		return index
	}

	for {

		br := bufio.NewReader(bytes.NewReader(content[index:]))
		line, err := br.ReadString('\n') //读取一行
		if err != nil {

			return -2
		}

		if num := strings.Index(line, ","+time); num != -1 { //匹配到时间点
			//fmt.Println(line)
			return index
		} else {
			index = index + len(line)
		}
	}

}

//读取上一行
func read_prev(content []byte, i int) int {
	index := strings.LastIndex(string(content[0:i]), "\nZZZZ") //找到前一时间行
	if index == -1 {
		return i
	}

	return index + 1
}

//读取下一行
func read_next(content []byte, i int) int {

	index := strings.Index(string(content[i+15:]), "\nZZZZ") //找到时间行
	if index == -1 {
		return i
	}

	index = index + i + 16
	return index
}

//解析标题
func analysis_titile(content []byte) {

	br := bufio.NewReader(bytes.NewReader(content[0:20000]))
	i := 0
	for {
		line, err := br.ReadString('\n')
		if err != nil {
			return
		}

		line_split := strings.Split(strings.TrimSpace(line), ",")
		//fmt.Print("line_split: %v \n", line_split)

		if strings.Compare(string(line_split[0]), string("AAA")) == 0 {
			continue
		}

		if strings.Compare(string(line_split[0]), string("BBBP")) == 0 {
			break
		}

		flag := string(line_split[0])
		item[flag] = i //由于map只支持单数组，所以两个变量做关联 //map 支持多重数组

		var new_line_split []string

		if strings.Index(flag, "NET") >= 0 {
			for _, v := range line_split {
				v = strings.Replace(v, "read", "r", 1)
				v = strings.Replace(v, "write", "w", 1)
				new_line_split = append(new_line_split, v)
			}
			item_title[i] = new_line_split
		} else {
			item_title[i] = line_split
		}
		i++
	}

	fmt.Println("item:", item)
	fmt.Println("item_title", item_title)

	return
}

//打印
func tprint(p string) {

	/*
	   	if len(p) < 8 {
	   		fmt.Printf("%s\t", p)
	   	} else {
	   		fmt.Printf("%s ", p)
	       }
	*/

	fmt.Printf("%-8s ", p)

}

//显示当前页数据
func view_index(content []byte, index int) {

	var top = make(map[int]string, 500)
	var top_index = 0

	br := bufio.NewReader(bytes.NewReader(content[index:]))
	line, err := br.ReadString('\n') //读第一行
	if err != nil {
		checkErr(err)
	}

	cpu_flag := 0

	line_split := strings.Split(line, ",")
	line0 := line_split[0]
	if strings.Compare(string(line_split[0]), string("ZZZZ")) == 0 {

		fmt.Printf("\033[H\033[J") //清除屏幕
		fmt.Println("Time:", line_split[2], line_split[3])
	}

	for {

		line, err := br.ReadString('\n') //读下一行
		if err != nil {
			break
		}

		line_split = strings.Split(strings.TrimSpace(line), ",")
		line0 = line_split[0]
		if strings.Compare(string(line0), string("ZZZZ")) == 0 { //下一页
			break
		}

		//获取CPU 使用率
		if strings.Index(line0, string("CPU0")) != -1 || strings.Index(line0, string("CPU1")) != -1 {
			if b_cpu {
				if cpu_flag == 0 {
					cpu_titile_index := item[line0]
					cpu_title := item_title[cpu_titile_index]
					for i := 0; i < len(cpu_title); i++ {
						if i <= 1 {
							fmt.Printf("\t")
							continue
						}
						tprint(cpu_title[i])
					}
					fmt.Printf("\n")
					cpu_flag = 1
				}
				fmt.Printf("\t")
				for i := 0; i < len(line_split); i++ {
					if i == 1 {
						continue
					}
					tprint(line_split[i])
				}
				fmt.Printf("\n")
			}

			continue
		}

		//获取top信息
		if strings.Index(line0, string("TOP")) != -1 {
			top[top_index] = line
			top_index++
			continue
		}

		//if strings.Index(line0, string("MEM")) != -1 {
		//     fmt.Println(item_title[8])
		//     fmt.Println(line)
		//}

		//打印其他
		if title_index, ok := item[line0]; ok {
			title := item_title[title_index]
			//fmt.Println(title)
			fmt.Print("\t")
			for i := 0; i < len(title); i++ {
				if i == 1 {
					continue
				}
				tprint(title[i])
			}
			fmt.Printf("\n\t\t")
			for i := 0; i < len(line_split); i++ {
				if i <= 1 {
					continue
				}
				tprint(line_split[i])
			}
			fmt.Printf("\n")
		}

	}

	//打印top
	if top_index != 0 {

		cpu_title_index := item["TOP"]
		cpu_title := item_title[cpu_title_index]

		fmt.Printf("\t")
		for i := 0; i < len(cpu_title); i++ {
			if i == 2 {
				continue
			}
			tprint(cpu_title[i])
		}
		fmt.Printf("\n")
		//最多打印t_cpu_num行
		if top_index > t_cpu_num {
			top_index = t_cpu_num
		}
		var view_num = 0
		for j := 0; j < len(top); j ++ {
            
            line=top[j]
            if line == "" {
                continue
            }

			if search != "" && !strings.Contains(line, search) {
				continue
			}
			if view_num >= top_index {
				break
			}
			view_num++
			line_split = strings.Split(strings.TrimSpace(line), ",")

			fmt.Printf("\t\t")
			for ii := 0; ii < len(line_split); ii++ {
				if ii == 0 || ii == 2 {
					continue
				}
				if strings.Index(line_split[ii], "/docker/") == 0 {
					tprint(strings.Replace(line_split[ii], "/docker/", "", 1)[0:12]) /* /docker/612745a041ab490e6358dcb1e8821d77d5d619f4e38e226e228a9b55033eec97 */
					continue
				}
				if strings.Index(line_split[ii], "/kubepods/") == 0 { /* /kubepods/pod479793d1-33ef-11e9-9c1f-e04f43ca8e14/c3d5ddee8f888f12d283c407b6eef72fea9fb583a3289c765ab9cc8cec2cd84f  */
					//
					c_split := strings.Split(line_split[ii], "/")
					if len(c_split) >= 4 {
						tprint(c_split[3][0:12])
						continue
					}
				}
				tprint(line_split[ii])
			}
			fmt.Printf("\n")
		}
	}

	return

}

//检查输入时间格式
func check_time(time string) bool {

	time_split := strings.Split(time, ":")

	if len(time_split) <= 1 {
		return false
	}

	hour, err := strconv.Atoi(time_split[0])
	if err != nil {
		return false
	}

	//fmt.Println(time_split)
	min, err := strconv.Atoi(time_split[1])
	if err != nil {
		return false
	}

	//fmt.Println("hour:", hour, "min:", min)
	if hour > 24 || min >= 60 {

		return false
	}
	return true
}

// 子函数 跳转进度
func time_jump(content []byte, index int) {

	for {
		key := getch()
		//fmt.Println("key:", key)
		switch key {
		case 113: //q
			c_k <- 1 //退出程序
		case 3: //Crtl+C
			c_k <- 1 //退出程序
		case 111: // o  //收缩cpu显示
			b_cpu = !b_cpu
			view_index(content, index)
		case 114: //r   //后退
			n_index := read_prev(content, index)
			view_index(content, n_index)
			index = n_index
		case 116: //t 下一页
			n_index := read_next(content, index)
			view_index(content, n_index)
			index = n_index

		case 107: //k 显示全top
			if t_cpu_num == 80 {
				t_cpu_num = 10
			} else {
				t_cpu_num = 80
			}

			view_index(content, index)
		case 58: //: 跳转时间
			fmt.Printf("Please input hh:mm=  ")
			input := bufio.NewReader(os.Stdin)
			time, _ := input.ReadString('\n')
			time = strings.TrimSpace(time)
			if check_time(time) {
				index = read_time(content, time)
				if index < 0 {
					fmt.Printf("\bNot fonud this time.")
				} else {
					view_index(content, index)
				}
			} else {
				fmt.Printf("\bTime format error.\n")
			}
		case 47: // /查找
			fmt.Printf("Please input process search character:  ")
			input := bufio.NewReader(os.Stdin)
			search, _ = input.ReadString('\n')
			search = strings.TrimSpace(search)
			view_index(content, index)
		default:
			break
		}
	}
}

func getch() int {
	t, _ := term.Open("/dev/tty")
	defer func() {
		t.Restore()
		t.Close()
		//fmt.Println("")
	}()
	//fmt.Println("With pkg/term")
	term.RawMode(t)
	t.SetReadTimeout(1)

	bytes := make([]byte, 1)
	numBytes, _ := t.Read(bytes)
	//fmt.Println("numBytes:",numBytes)
	if numBytes != 0 {
		//fmt.Println("bytes:", bytes)
		return int(bytes[0])
	}

	return 0

}

func usage() {

	fmt.Println("Please set filename.")
	fmt.Println(" eg: ./nmon_read -f filename")
	str := ` 
    o  显示或者收起单个cpu使用率，防止cpu太多导致屏幕显示不全
    t  时间前进
    r  时间后退
    q  退出
    :  跳转到文件记录时间(输入: 01:20 1点20分)
    k  显示top 10 或者所有top进程
    /  匹配进程信息，为空，显示所有进程信息
        `
	fmt.Println(str)
	os.Exit(1)
}

func main() {

	flag.StringVar(&filename, "f", "", "-f filename 指定文件")
	flag.Usage = usage
	flag.Parse()

	if len(filename) == 0 {
		flag.Usage()
	}

	content, err := ioutil.ReadFile(filename)
	checkErr(err)

	analysis_titile(content)

	index := read_time(content, "")
	//index := read_time(content, "14:46")
	//println("index:", index)
	if index <= -1 {
		fmt.Println("   Not find time flag.")
		os.Exit(1)
	}
	t, _ := term.Open("/dev/tty")
	defer func() {
		t.Restore()
		t.Close()
	}()

	view_index(content, index)
	//fmt.Println(index)
	c_k = make(chan int)
	go time_jump(content, index)
	<-c_k

}
