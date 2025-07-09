class OkerCompiler {
    constructor() {
        this.initializeElements();
        this.attachEventListeners();
        this.loadExamples();
    }

    initializeElements() {
        this.sourceCode = document.getElementById('source-code');
        this.output = document.getElementById('output');
        this.compileBtn = document.getElementById('compile-btn');
        this.tokensBtn = document.getElementById('tokens-btn');
        this.astBtn = document.getElementById('ast-btn');
        this.bytecodeBtn = document.getElementById('bytecode-btn');
        this.clearBtn = document.getElementById('clear-btn');
        this.clearOutputBtn = document.getElementById('clear-output-btn');
        this.exampleSelect = document.getElementById('example-select');
        this.saveBtn = document.getElementById('save-btn');
        this.loadBtn = document.getElementById('load-btn');
        
        this.tokensPanel = document.getElementById('tokens-panel');
        this.astPanel = document.getElementById('ast-panel');
        this.bytecodePanel = document.getElementById('bytecode-panel');
        this.tokensOutput = document.getElementById('tokens-output');
        this.astOutput = document.getElementById('ast-output');
        this.bytecodeOutput = document.getElementById('bytecode-output');
    }

    attachEventListeners() {
        this.compileBtn.addEventListener('click', () => this.compileAndRun());
        this.tokensBtn.addEventListener('click', () => this.showTokens());
        this.astBtn.addEventListener('click', () => this.showAST());
        this.bytecodeBtn.addEventListener('click', () => this.showBytecode());
        this.clearBtn.addEventListener('click', () => this.clearAll());
        this.clearOutputBtn.addEventListener('click', () => this.clearOutput());
        this.exampleSelect.addEventListener('change', (e) => this.loadExample(e.target.value));
        this.saveBtn.addEventListener('click', () => this.saveCode());
        this.loadBtn.addEventListener('click', () => this.loadCode());
        
        // Keyboard shortcuts
        document.addEventListener('keydown', (e) => {
            if (e.ctrlKey || e.metaKey) {
                switch(e.key) {
                    case 'Enter':
                        e.preventDefault();
                        this.compileAndRun();
                        break;
                    case 's':
                        e.preventDefault();
                        this.saveCode();
                        break;
                    case 'o':
                        e.preventDefault();
                        this.loadCode();
                        break;
                }
            }
        });
    }

    loadExamples() {
        this.examples = {
            hello: `~ Simple hello world program
say "Hello, World!"

~ Variables and basic operations
let name = "Alice"
let age = 25
say "Hello, " + name + "! You are " + str(age) + " years old."

~ Math operations
let a = 10
let b = 3
say "Addition: " + str(a + b)
say "Subtraction: " + str(a - b)
say "Multiplication: " + str(a * b)
say "Division: " + str(a / b)
say "Modulo: " + str(a % b)`,

            factorial: `~ Factorial calculation using recursion
makef factorial(n):
    if n <= 1:
        return 1
    else:
        return n * factorial(n - 1)
    end
end

~ Test factorial function
let n = 5
let result = factorial(n)
say str(n) + "! = " + str(result)

~ Calculate factorials for numbers 1 to 5
say "Factorials from 1 to 5:"
let i = 1
while i <= 5:
    let fact = factorial(i)
    say str(i) + "! = " + str(fact)
    let i = i + 1
end`,

            fibonacci: `~ Fibonacci sequence generator
makef fibonacci(n):
    if n <= 1:
        return n
    else:
        return fibonacci(n - 1) + fibonacci(n - 2)
    end
end

~ Generate first 10 Fibonacci numbers
say "First 10 Fibonacci numbers:"
let i = 0
while i < 10:
    let fib = fibonacci(i)
    say "F(" + str(i) + ") = " + str(fib)
    let i = i + 1
end`,

            class: `~ Class-like example using functions
~ Person "constructor" function
makef create_person(name, age):
    say "Creating person: " + name + " (age " + str(age) + ")"
    return name + "_" + str(age)
end

~ Person methods
makef person_greet(name, age):
    say "Hello, I'm " + name + " and I'm " + str(age) + " years old."
end

makef person_birthday(name, age):
    let new_age = age + 1
    say name + " is now " + str(new_age) + " years old! Happy birthday!"
    return new_age
end

~ Create and use "person objects"
let alice = create_person("Alice", 25)
person_greet("Alice", 25)
let alice_new_age = person_birthday("Alice", 25)
person_greet("Alice", alice_new_age)`
        };
    }

    async compileAndRun() {
        const code = this.sourceCode.value.trim();
        if (!code) {
            this.showError('Please enter some code to compile.');
            return;
        }

        this.showLoading('Compiling and running...');
        
        try {
            const response = await fetch('/compile', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ code, action: 'run' })
            });

            const result = await response.json();
            
            if (result.success) {
                this.showOutput(result.output || 'Program executed successfully.');
            } else {
                this.showError(result.error || 'Compilation failed.');
            }
        } catch (error) {
            this.showError('Connection error: ' + error.message);
        }
    }

    async showTokens() {
        const code = this.sourceCode.value.trim();
        if (!code) {
            this.showError('Please enter some code to analyze.');
            return;
        }

        this.showLoading('Analyzing tokens...');
        
        try {
            const response = await fetch('/compile', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ code, action: 'tokens' })
            });

            const result = await response.json();
            
            if (result.success) {
                this.tokensOutput.textContent = result.output || 'No tokens generated.';
                this.tokensPanel.style.display = 'block';
                this.tokensPanel.scrollIntoView({ behavior: 'smooth' });
            } else {
                this.showError(result.error || 'Token analysis failed.');
            }
        } catch (error) {
            this.showError('Connection error: ' + error.message);
        }
    }

    async showAST() {
        const code = this.sourceCode.value.trim();
        if (!code) {
            this.showError('Please enter some code to analyze.');
            return;
        }

        this.showLoading('Generating AST...');
        
        try {
            const response = await fetch('/compile', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ code, action: 'ast' })
            });

            const result = await response.json();
            
            if (result.success) {
                this.astOutput.textContent = result.output || 'No AST generated.';
                this.astPanel.style.display = 'block';
                this.astPanel.scrollIntoView({ behavior: 'smooth' });
            } else {
                this.showError(result.error || 'AST generation failed.');
            }
        } catch (error) {
            this.showError('Connection error: ' + error.message);
        }
    }

    async showBytecode() {
        const code = this.sourceCode.value.trim();
        if (!code) {
            this.showError('Please enter some code to analyze.');
            return;
        }

        this.showLoading('Generating bytecode...');
        
        try {
            const response = await fetch('/compile', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ code, action: 'bytecode' })
            });

            const result = await response.json();
            
            if (result.success) {
                this.bytecodeOutput.textContent = result.output || 'No bytecode generated.';
                this.bytecodePanel.style.display = 'block';
                this.bytecodePanel.scrollIntoView({ behavior: 'smooth' });
            } else {
                this.showError(result.error || 'Bytecode generation failed.');
            }
        } catch (error) {
            this.showError('Connection error: ' + error.message);
        }
    }

    clearAll() {
        this.sourceCode.value = '';
        this.clearOutput();
        this.hideDebugPanels();
        this.exampleSelect.value = '';
    }

    clearOutput() {
        this.output.innerHTML = '';
    }

    hideDebugPanels() {
        this.tokensPanel.style.display = 'none';
        this.astPanel.style.display = 'none';
        this.bytecodePanel.style.display = 'none';
    }

    loadExample(exampleName) {
        if (exampleName && this.examples[exampleName]) {
            this.sourceCode.value = this.examples[exampleName];
            this.clearOutput();
            this.hideDebugPanels();
        }
    }

    saveCode() {
        const code = this.sourceCode.value;
        const blob = new Blob([code], { type: 'text/plain' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = 'program.oker';
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        URL.revokeObjectURL(url);
    }

    loadCode() {
        const input = document.createElement('input');
        input.type = 'file';
        input.accept = '.oker,.txt';
        input.onchange = (e) => {
            const file = e.target.files[0];
            if (file) {
                const reader = new FileReader();
                reader.onload = (e) => {
                    this.sourceCode.value = e.target.result;
                    this.clearOutput();
                    this.hideDebugPanels();
                };
                reader.readAsText(file);
            }
        };
        input.click();
    }

    showOutput(message) {
        this.output.innerHTML = `<div class="success">${this.escapeHtml(message)}</div>`;
    }

    showError(message) {
        this.output.innerHTML = `<div class="error">${this.escapeHtml(message)}</div>`;
    }

    showLoading(message) {
        this.output.innerHTML = `<div class="loading">${this.escapeHtml(message)}</div>`;
    }

    escapeHtml(text) {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }
}

// Initialize the compiler when the page loads
document.addEventListener('DOMContentLoaded', () => {
    new OkerCompiler();
});
