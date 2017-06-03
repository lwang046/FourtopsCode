import sys

def progress(current, total, status=''):
	fullBarLength = 80
	doneBarLength = int(round(fullBarLength * current / float(total)))

	percents = round(100.0 * current / float(total), 1)
	bar = '>' * doneBarLength + ' ' * (fullBarLength - doneBarLength)

	sys.stdout.write('[%s] %s%s ...%s\r' % (bar, percents, '%', status))
	sys.stdout.flush()
