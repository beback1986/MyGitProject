
class Tester:
    def __init__(self, name):
        self.name = name

    def get_name(self):
        return self.name


def initialiser(name, class_type):
    return class_type(name)



a = initialiser("hello", Tester)
b = initialiser("nihao", Tester)
print a.get_name()
print b.get_name()
