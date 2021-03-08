
def average(array):
    return sum(array) / len(array) 

def show_average(one_thread, two_threads, four_threads, eight_threads):
    print(average(one_thread))
    print(average(two_threads))
    print(average(four_threads))
    print(average(eight_threads))

one_thread = []

two_threads = []

four_threads = []

eight_threads = []

show_average(one_thread, two_threads, four_threads, eight_threads)