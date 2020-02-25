# Makefile

FLAGS = -w -W -u -k -O

all:			semaphore.tin crossroad.tin advanced_crossroad.tin tinybenchmark.tin benchmark.tin benchmark_8.tin benchmark_16.tin enterprise.tin excl.tin mutex_4.tin resource.tin multiplexer.tin cs.tin wm.tin outchar.tin blanket.tin parbench.tin automa.tin

semaphore.tin:		semaphore.btl
			ting $(FLAGS) -o semaphore.tin semaphore.btl

crossroad.tin:		crossroad.btl
			ting $(FLAGS) -o crossroad.tin crossroad.btl

advanced_crossroad.tin:	advanced_crossroad.btl
			ting $(FLAGS) -o advanced_crossroad.tin advanced_crossroad.btl

tinybenchmark.tin:	tinybenchmark.btl
			ting $(FLAGS) -o tinybenchmark.tin tinybenchmark.btl

benchmark.tin:		benchmark.btl
			ting $(FLAGS) -o benchmark.tin benchmark.btl

benchmark_8.tin:	benchmark_8.btl
			ting $(FLAGS) -o benchmark_8.tin benchmark_8.btl

benchmark_16.tin:	benchmark_16.btl
			ting $(FLAGS) -o benchmark_16.tin benchmark_16.btl

enterprise.tin:		enterprise.btl
			ting $(FLAGS) -o enterprise.tin enterprise.btl

excl.tin:		excl.btl
			ting $(FLAGS) -o excl.tin excl.btl

mutex_4.tin:		mutex_4.btl
			ting $(FLAGS) -o mutex_4.tin mutex_4.btl

resource.tin:		resource.btl
			ting $(FLAGS) -o resource.tin resource.btl

multiplexer.tin:	multiplexer.btl
			ting $(FLAGS) -o multiplexer.tin multiplexer.btl

cs.tin:			cs.btl
			ting $(FLAGS) -o cs.tin cs.btl

wm.tin:			wm.btl
			ting $(FLAGS) -o wm.tin wm.btl

outchar.tin:		outchar.btl
			ting $(FLAGS) -o outchar.tin outchar.btl

blanket.tin:		blanket.btl
			ting $(FLAGS) -o blanket.tin blanket.btl

parbench.tin:		parbench.btl
			ting $(FLAGS) -o parbench.tin parbench.btl

automa.tin:		automa.btl
			ting $(FLAGS) -o automa.tin automa.btl
clean:
			rm -f *.tin *.evl *~

