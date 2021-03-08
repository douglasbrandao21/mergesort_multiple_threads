
def average(array):
    return sum(array) / len(array) 

def show_average(one_thread, two_threads, four_threads, eight_threads):
    print(average(one_thread))
    print(average(two_threads))
    print(average(four_threads))
    print(average(eight_threads))

one_thread = [
    0.181424596,
    0.184793595,
    0.187505657,
    0.184638603,
    0.181466872
]

two_threads = [
    0.093241351,
    0.093301685,
    0.094641766,
    0.094111023,
    0.096014946
]

four_threads = [
    0.058630339,
    0.060143143,
    0.072533899,
    0.055457656,
    0.054960543
]

eight_threads = [
    0.062181951,
    0.064994971,
    0.065319335,
    0.065539110,
    0.064111551
]

show_average(one_thread, two_threads, four_threads, eight_threads)