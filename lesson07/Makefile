#定义变量
src=$(wildcard ./*.c)
target=app
objs=$(patsubst %.c, %.o, $(src))
$(target):$(objs)
	$(CC) $(objs) -o $(target)
	
%.o:%.c
	$(CC) -c $< -o $@

.PHONY:clean
clean:
	rm $(objs) -f

