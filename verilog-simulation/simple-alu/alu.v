// 实现一个简单的四位ALU。

// | 功能选择 | 功能 | 操作 |
// | :--- | :--- | :--- |
// | 000 | 加法 | A+B |
// | 001 | 减法 | A-B |
// | 010 | 取反 | Not A |
// | 011 | 与 | A and B |
// | 100 | 或 | A or B |
// | 101 | 异或 | A xor B |
// | 110 | 比较大小 | If A>B then out = 4’b0001; else out = 4’b0000; |
// | 111 | 判断相等 | If A == B then out            = 4’b0001; else out            = 4’b0000; |

// ALU 的输入是两个四位二进制补码数 A 和 B，三位功能选择信号ALUctr，具体功能对应参见上。输出是运算结果，及对应的进位位，溢出位和 Zero 指示位。
// 在实现此 ALU 的时候，请同时输出加减运算的进位位 C 和溢出位 overflow 位 的输出。(一般情况下，涉及加减运算的，可以按照加减运算器来考虑进位位和溢出位;涉及逻辑运算的，可以直接设置进位位和溢出位为 “0”。比较大小时需要考虑符号位。)

module alu_s(input [3:0] A,      // 输入四位补码操作数A
             input [3:0] B,      // 输入四位补码操作数B
             input [2:0] ALUctr, // ALU控制端，3位
             output reg [3:0] F, // 输出四位补码结果
             output reg cf,      // 一位进位
             output reg zero,    // 一位零位
             output reg of);     // 一位溢出

wire sub_mode = (ALUctr == 3'b001) || (ALUctr == 3'b110) || (ALUctr == 3'b111);
wire [3:0] adder_F;
wire adder_cf, adder_zero, adder_of;

adder u_adder(
.A(A),
.B(B),
.addsub(sub_mode),
.F(adder_F),
.cf(adder_cf),
.zero(adder_zero),
.of(adder_of)
);

always @(*) begin
    cf   = 0;
    of   = 0;
    zero = 0;
    case (ALUctr)
        3'b000: begin // Add
            F    = adder_F;
            cf   = adder_cf;
            of   = adder_of;
            zero = adder_zero;
        end
        3'b001: begin // Sub
            F    = adder_F;
            cf   = adder_cf;
            of   = adder_of;
            zero = adder_zero;
        end
        3'b010: begin // Not
            F    = ~A;
            zero = (F == 4'b0000);
        end
        3'b011: begin // And
            F    = A & B;
            zero = (F == 4'b0000);
        end
        3'b100: begin // Or
            F    = A | B;
            zero = (F == 4'b0000);
        end
        3'b101: begin // Xor
            F    = A ^ B;
            zero = (F == 4'b0000);
        end
            3'b110: begin // Compare A > B
                if ((adder_F[3] == adder_of) && !adder_zero) begin
                    F = 4'b0001;
                end else begin
                    F = 4'b0000;
                end
                zero = (F == 4'b0000);
            end
            3'b111: begin // Equal
                if (adder_zero) begin
                    F = 4'b0001;
                end else begin
                    F = 4'b0000;
                end
                zero = (F == 4'b0000);
            end
        default: begin
            F    = 4'b0000;
            zero = 1;
        end
    endcase
end

endmodule

module adder(
    input [3:0] A,  // 被加数或被减数
    input [3:0] B,  // 加数或减数
    input addsub,   // 为0时计算加法，为1时计算减法
    output [3:0] F, // A+B或A-B
    output cf,      // 进位/借位标志
    output zero,    // 结果为0标志
    output of     // 溢出标志
    );
    
    wire [3:0] B_mod;  // B的修改版，用于加减法运算
    assign B_mod = B ^ {4{addsub}}; // 如果是减法，则取B的补码
    
    wire [4:0] sum;    // 5位宽的和，用于存储进位
    assign sum = {1'b0, A} + {1'b0, B_mod} + {4'b0, addsub}; // 计算A和修改后的B的和
    
    assign F    = sum[3:0]; // 结果的低4位
    assign cf   = sum[4] ^ addsub; // 进位标志，减法时按X86定义
    assign zero = (F == 4'b0000) ? 1'b1 : 1'b0; // 结果为0标志
    
    assign of = (A[3] == B_mod[3]) && (F[3] != A[3]); // 溢出标志
    
endmodule
