from matplotlib import pyplot as pl

iteracoes = []
clausulas = []

with open('SA-250.csv', 'r') as dataset:
	for line in dataset.readlines():
		line = line.strip()
		x, y = line.split(',')
		iteracoes.append(float(x))
		clausulas.append(float(y))


pl.plot(iteracoes, clausulas)

pl.title('Simulated Annealing (250 átomos) ')
pl.xlabel('Iterações')
pl.ylabel('Clausulas verdadeiras')
pl.ylim([0,1065])
pl.xlim([0,250000])

pl.show()
