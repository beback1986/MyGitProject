
class OuterClass {
	private String str = "OuterClass member";

	interface InnerInterface {
		String str = "InnerInterface member";
		int value();
		String string();
	}

	class InnerClass {
		String string() {
			return str;
		}
	}

	public String getString() {
		return str;
	}
}


public class Tester {
	public static void main(String[] args) {
		OuterClass o = new OuterClass();
		final String str = "Invoker Local Variable";

		OuterClass.InnerInterface i = new OuterClass.InnerInterface() {
			public int value() {
				return 123;
			}
			public String string() {
				return str;
			}
		};
		OuterClass.InnerClass c = o.new InnerClass();

		System.out.println(i.value());
		System.out.println(i.string());
	}
}
