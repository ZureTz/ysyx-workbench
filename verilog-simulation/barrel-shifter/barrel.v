module barrel(input [31:0] indata,
              input [4:0] shamt,
              input lr,
              input al,
              output reg [31:0] outdata);

    reg [31:0] t0, t1, t2, t3;

    always @(*) begin
        // Stage 0: Shift 1
        if (shamt[0]) begin
            if (lr) begin // Left
                t0 = {indata[30:0], 1'b0};
            end else begin // Right
                t0 = { (al ? indata[31] : 1'b0), indata[31:1] };
            end
        end else begin
            t0 = indata;
        end

        // Stage 1: Shift 2
        if (shamt[1]) begin
            if (lr) begin // Left
                t1 = {t0[29:0], 2'b0};
            end else begin // Right
                t1 = { (al ? {2{t0[31]}} : 2'b0), t0[31:2] };
            end
        end else begin
            t1 = t0;
        end

        // Stage 2: Shift 4
        if (shamt[2]) begin
            if (lr) begin // Left
                t2 = {t1[27:0], 4'b0};
            end else begin // Right
                t2 = { (al ? {4{t1[31]}} : 4'b0), t1[31:4] };
            end
        end else begin
            t2 = t1;
        end

        // Stage 3: Shift 8
        if (shamt[3]) begin
            if (lr) begin // Left
                t3 = {t2[23:0], 8'b0};
            end else begin // Right
                t3 = { (al ? {8{t2[31]}} : 8'b0), t2[31:8] };
            end
        end else begin
            t3 = t2;
        end

        // Stage 4: Shift 16
        if (shamt[4]) begin
            if (lr) begin // Left
                outdata = {t3[15:0], 16'b0};
            end else begin // Right
                outdata = { (al ? {16{t3[31]}} : 16'b0), t3[31:16] };
            end
        end else begin
            outdata = t3;
        end
    end

endmodule
