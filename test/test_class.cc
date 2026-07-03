#include <iostream>
#include <string>
#include <vector>

#include "lox/core/scanner.h"
#include "lox/core/parser.h"
#include "lox/core/resolver.h"
#include "lox/core/lox.h"
#include "lox/ast/visitors/interpreter.h"

namespace lox {
namespace test {

static bool RunSource(const std::string& source, bool expect_success) {
  Scanner scanner(source);
  auto tokens = scanner.ScanTokens();

  Parser parser(tokens);
  auto statements = parser.Parse();
  if (Lox::Instance().HadError()) {
    Lox::Instance().ResetErrors();
    return !expect_success;
  }

  Interpreter interpreter;
  Resolver resolver(interpreter);
  resolver.Resolve(statements);
  if (Lox::Instance().HadError()) {
    Lox::Instance().ResetErrors();
    return !expect_success;
  }

  interpreter.Interpret(std::move(statements));
  if (Lox::Instance().HadRuntimeError()) {
    Lox::Instance().ResetErrors();
    return !expect_success;
  }

  return expect_success;
}

struct TestCase {
  std::string name;
  std::string source;
  bool expect_success;
};

void testClass() {
  std::cout << "\n🧬 类继承测试\n";
  std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

  std::vector<TestCase> tests;

  // ============ 回归测试：基本类（无继承） ============
  tests.push_back({"基本类声明（无继承）", R"(
class Simple {
  sayHi() { return "hello"; }
}
var s = Simple();
print s.sayHi();
)", true});

  tests.push_back({"类带 init", R"(
class Person {
  init(name) { this.name = name; }
}
var p = Person("Alice");
print p.name;
)", true});

  // ============ 继承成功用例 ============
  tests.push_back({"子类继承父类方法", R"(
class Animal {
  speak() { return "roar"; }
}
class Dog < Animal {}
var d = Dog();
print d.speak();
)", true});

  tests.push_back({"子类方法覆盖父类方法", R"(
class Animal {
  speak() { return "generic"; }
}
class Cat < Animal {
  speak() { return "meow"; }
}
var c = Cat();
print c.speak();
)", true});

  tests.push_back({"super 调用父类方法", R"(
class Animal {
  speak() { return "generic"; }
}
class Dog < Animal {
  speak() { return "woof"; }
  parentSpeak() { return super.speak(); }
}
var d = Dog();
print d.speak();
print d.parentSpeak();
)", true});

  tests.push_back({"super 调用带参数的父类方法", R"(
class Base {
  add(a, b) { return a + b; }
}
class Derived < Base {
  add(a, b) { return a + b + 1; }
  baseAdd(a, b) { return super.add(a, b); }
}
var d = Derived();
print d.baseAdd(3, 4);
)", true});

  tests.push_back({"三级继承链", R"(
class A {
  a() { return "A"; }
}
class B < A {}
class C < B {}
var c = C();
print c.a();
)", true});

  tests.push_back({"三级继承链中间覆盖", R"(
class A {
  name() { return "A"; }
}
class B < A {
  name() { return "B"; }
}
class C < B {}
var c = C();
print c.name();
)", true});

  tests.push_back({"super 调用父类 init", R"(
class Animal {
  init(name) { this.name = name; }
}
class Dog < Animal {
  init(name, breed) {
    super.init(name);
    this.breed = breed;
  }
}
var d = Dog("Rex", "Shepherd");
print d.name;
print d.breed;
)", true});

  tests.push_back({"子类继承父类 getter", R"(
class Base {
  greeting { return "hello"; }
}
class Derived < Base {}
var d = Derived();
print d.greeting;
)", true});

  tests.push_back({"子类覆盖父类 getter", R"(
class Base {
  greeting { return "hello"; }
}
class Derived < Base {
  greeting { return "hi"; }
}
var d = Derived();
print d.greeting;
)", true});

  tests.push_back({"super 访问父类 getter（通过方法间接）", R"(
class Base {
  greeting { return "hello"; }
}
class Derived < Base {
  parentGreeting() { return super.greeting; }
}
var d = Derived();
print d.parentGreeting();
)", true});

  tests.push_back({"子类继承父类静态方法", R"(
class Base {
  class factory() { return Base(); }
}
class Derived < Base {}
var d = Derived.factory();
print d;
)", true});

  tests.push_back({"子类方法中使用 this", R"(
class Animal {
  init(name) { this.name = name; }
  getName() { return this.name; }
}
class Dog < Animal {}
var d = Dog("Buddy");
print d.getName();
)", true});

  tests.push_back({"super 调用后子类继续执行", R"(
class Base {
  desc() { return "base"; }
}
class Derived < Base {
  desc() {
    var base = super.desc();
    return base + "-derived";
  }
}
var d = Derived();
print d.desc();
)", true});

  // ============ 错误用例 ============
  tests.push_back({"类继承自身应报错", R"(
class Foo < Foo {}
)", false});

  tests.push_back({"super 在类外使用应报错", R"(
super.foo();
)", false});

  tests.push_back({"super 在无父类的类中使用应报错", R"(
class Foo {
  bar() { return super.bar(); }
}
)", false});

  tests.push_back({"父类必须是类（不是字符串）", R"(
var x = "not a class";
class Foo < x {}
)", false});

  tests.push_back({"super 调用不存在的方法应运行时错误", R"(
class Base {}
class Derived < Base {
  bad() { return super.nope(); }
}
var d = Derived();
print d.bad();
)", false});

  int passed = 0;
  int failed = 0;

  for (auto& t : tests) {
    std::cout << "  测试: " << t.name;
    if (t.expect_success) {
      std::cout << " (应成功)";
    } else {
      std::cout << " (应报错)";
    }
    std::cout << "\n";

    bool result = RunSource(t.source, t.expect_success);
    if (result) {
      std::cout << "    ✅ 通过\n\n";
      passed++;
    } else {
      std::cout << "    ❌ 失败\n\n";
      failed++;
    }
  }

  std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
  std::cout << "类继承测试: " << passed << "/" << (passed + failed) << " 通过\n";
  if (failed == 0) {
    std::cout << "✅ 类继承所有测试通过！\n";
  }
}

}  // namespace test
}  // namespace lox
